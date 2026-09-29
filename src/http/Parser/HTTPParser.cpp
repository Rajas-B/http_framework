#include "HTTPParser.hpp"
#include <cstring>
#include <algorithm>
#include <iostream>

#include "http/Data/Request.hpp"
#include "http/Codes/ReadingCodes.hpp"


HTTPParser::HTTPParser() {
    reset_state();
}

void HTTPParser::reset_state() {
    request = std::make_unique<Request>();
    state = ReadingHeaders;
}

ReadingCodes HTTPParser::consume(const uint8_t* buf, size_t length, size_t& total_consumed) {
    total_consumed = 0;
    if (length == 0) return ReadingCodes::ERROR;

    // after return_request, the unique_ptr will be null, so create new one
    if (!request) {
        request = std::make_unique<Request>();
        state = ReadingHeaders;
    }

    while (total_consumed < length) {
        size_t step_consumed = 0;

        if (state == ReadingHeaders) {
            ReadingCodes code = read_headers(buf + total_consumed, length - total_consumed, step_consumed);
            if (code != ReadingCodes::OK) return code;
            
            total_consumed += step_consumed; 
        } 
        else if (state == ReadingBody) {
            ReadingCodes code = read_body(buf + total_consumed, length - total_consumed, step_consumed);
            if (code != ReadingCodes::OK) return code;
            
            total_consumed += step_consumed;

            if (request->body->body_length == request->body->content_length) {
                return ReadingCodes::COMPLETED; // handler should take of dispatching request
            }
        }
    }
    
    // consumed the whole buffer but the request isn't finished yet.
    return ReadingCodes::OK; 
}

ReadingCodes HTTPParser::read_headers(const uint8_t* buf, size_t length, size_t& consumed) {
    auto& headers = request->headers;

    if (headers->bytes_accumulated + length > Headers::k_max_header_size) {
        return ReadingCodes::ERROR; // header is too large, http code 431
    }

    // copy all incoming bytes to buffer first, avoids split delimiter issues
    std::memcpy(headers->headers_buf + headers->bytes_accumulated, buf, length);
    headers->bytes_accumulated += length;

    // search the entire accumulated view
    std::string_view buffer_view(reinterpret_cast<const char*>(headers->headers_buf), headers->bytes_accumulated);
    constexpr std::string_view double_crlf = "\r\n\r\n";
    size_t end_of_headers_pos = buffer_view.find(double_crlf);

    if (end_of_headers_pos != std::string_view::npos) {
        // headers finished
        size_t header_block_size = end_of_headers_pos + double_crlf.length();
        
        std::string_view headers_only = buffer_view.substr(0, end_of_headers_pos);
        if (parse_headers(headers_only) == ReadingCodes::ERROR) return ReadingCodes::ERROR;
        if (validate_headers() == ReadingCodes::ERROR) return ReadingCodes::ERROR;
        if (process_headers() == ReadingCodes::ERROR) return ReadingCodes::ERROR;

        size_t bytes_from_buf_consumed = header_block_size - (headers->bytes_accumulated - length);
        
        consumed = bytes_from_buf_consumed;
        state = ReadingBody;
    } else {
        // header not finished yet, we consumed the entire chunk
        consumed = length; 
    }

    return ReadingCodes::OK;
}

ReadingCodes HTTPParser::parse_headers(std::string_view headers_view) {
    constexpr std::string_view crlf = "\r\n";
    
    size_t line_end = headers_view.find(crlf);
    if (line_end == std::string_view::npos) return ReadingCodes::ERROR;
    
    if (parse_request_line(headers_view.substr(0, line_end)) == ReadingCodes::ERROR) {
        return ReadingCodes::ERROR;
    }
    
    headers_view.remove_prefix(line_end + crlf.length());

    while (!headers_view.empty()) {
        line_end = headers_view.find(crlf);
        std::string_view line = headers_view.substr(0, line_end);
        
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string_view::npos) {
            std::string_view key = line.substr(0, colon_pos);
            size_t val_start = line.find_first_not_of(" \t", colon_pos + 1);
            std::string_view value = (val_start != std::string_view::npos) ? line.substr(val_start) : "";

            request->headers->parsed_headers.emplace(std::string(key), std::string(value));
        }

        if (line_end == std::string_view::npos) break; 
        headers_view.remove_prefix(line_end + crlf.length());
    }
    return ReadingCodes::OK;
}

ReadingCodes HTTPParser::parse_request_line(std::string_view line) {
    size_t first_space = line.find(' ');
    if (first_space == std::string_view::npos) return ReadingCodes::ERROR;
    
    size_t second_space = line.find(' ', first_space + 1);
    if (second_space == std::string_view::npos) return ReadingCodes::ERROR;
    
    request->method = std::string(line.substr(0, first_space));
    request->url = std::string(line.substr(first_space + 1, second_space - (first_space + 1)));
    request->version = std::string(line.substr(second_space + 1));
    std::string_view version_view(request->version);

    if (request->method.empty() || request->url.empty() || !version_view.starts_with("HTTP/")) {
        return ReadingCodes::ERROR;
    }
    return ReadingCodes::OK;
}

ReadingCodes HTTPParser::validate_headers() {
    const auto& headers_map = request->headers->parsed_headers;

    // HTTP/1.1 strictly requires host
    if (request->version == "HTTP/1.1") {
        if (headers_map.find("Host") == headers_map.end()) {
            return ReadingCodes::ERROR; // Should trigger a 400 Bad Request
        }
    }

    // HTTP Request Smuggling
    // request cannot have both Content-Length and Transfer-Encoding
    bool has_content_length = headers_map.find("Content-Length") != headers_map.end();
    bool has_transfer_encoding = headers_map.find("Transfer-Encoding") != headers_map.end();

    if (has_content_length && has_transfer_encoding) {
        return ReadingCodes::ERROR; // http code 400
    }

    return ReadingCodes::OK;
}

ReadingCodes HTTPParser::process_headers() {
    request->body = std::make_unique<Body>();
    
    auto it = request->headers->parsed_headers.find("Content-Length");
    if (it != request->headers->parsed_headers.end()) {
        try {
            request->body->content_length = std::stoull(it->second);
        } catch (...) {
            return ReadingCodes::ERROR; // malformed length error
        }
    } else {
        request->body->content_length = 0; // e.g., GET requests
    }
    
    return ReadingCodes::OK;
}

ReadingCodes HTTPParser::read_body(const uint8_t* buf, size_t length, size_t& consumed) {
    auto& body = request->body;
    
    // how many bytes of body still remaining
    size_t needed = body->content_length - body->body_length;
    
    // exact number of bytes we will use from this buffer
    consumed = std::min(length, needed);
    
    size_t to_copy = consumed;
    size_t offset = 0; // tracks position in the input buffer

    // copy in current frame and next if needed
    while (to_copy > 0) {
        size_t current_frame_idx = body->body_length / Body::k_frame_size;
        size_t offset_in_frame = body->body_length % Body::k_frame_size;
        size_t space_in_frame = Body::k_frame_size - offset_in_frame;

        // current frame is full, create next frame
        if (current_frame_idx >= body->frames.size()) {
            body->frames.push_back(new uint8_t[Body::k_frame_size]);
        }

        size_t chunk_size = std::min(to_copy, space_in_frame);
        
        // read from buf + offset
        std::memcpy(body->frames[current_frame_idx] + offset_in_frame, buf + offset, chunk_size);

        // advance all the pointers and counters
        offset += chunk_size;
        to_copy -= chunk_size;
        body->body_length += chunk_size;
    }

    return ReadingCodes::OK; 
}

std::unique_ptr<Request> HTTPParser::return_complete_request() {
    return std::move(this->request); 
}