#include "HTTPParser.hpp"
#include <cstring>
#include <algorithm>
#include <iostream>

#include "Status.hpp"
#include "Data/Request.hpp"

HTTPParser::HTTPParser() {
    reset_state();
}

void HTTPParser::reset_state() {
    request = std::make_unique<Request>();
    state = ReadingHeaders;
}

Status HTTPParser::consume(const uint8_t* buf, size_t length) {
    if (length == 0) return Status::ERROR;

    // The core Event Loop for the parser
    while (length > 0) {
        if (state == ReadingHeaders) {
            if (read_headers(buf, length) == Status::ERROR) {
                return Status::ERROR;
            }
        } 
        
        if (state == ReadingBody) {
            if (read_body(buf, length) == Status::ERROR) {
                return Status::ERROR;
            }

            // Check if the body is fully received
            if (request->body->body_length == request->body->content_length) {
                
                // ---- PLACEHOLDER: Dispatch to Router ----
                // Example: router->dispatch(std::move(request));
                // -----------------------------------------

                // Reset state to handle the next pipelined Keep-Alive request
                reset_state(); 
            }
        }
    }
    return Status::OK;
}

Status HTTPParser::read_headers(const uint8_t*& buf, size_t& length) {
    auto& headers = request->headers;

    if (headers->bytes_accumulated + length > Headers::k_max_header_size) {
        return Status::ERROR; // header is too large, http code 431
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
        if (parse_headers(headers_only) == Status::ERROR) return Status::ERROR;
        if (validate_headers() == Status::ERROR) return Status::ERROR;
        if (process_headers() == Status::ERROR) return Status::ERROR;

        // body bytes present
        size_t bytes_from_buf_consumed = header_block_size - (headers->bytes_accumulated - length);
        
        buf += bytes_from_buf_consumed;
        length -= bytes_from_buf_consumed;
        
        state = ReadingBody;
    } else {
        // header not finished yet
        buf += length;
        length = 0; 
    }

    return Status::OK;
}

Status HTTPParser::parse_headers(std::string_view headers_view) {
    constexpr std::string_view crlf = "\r\n";
    
    size_t line_end = headers_view.find(crlf);
    if (line_end == std::string_view::npos) return Status::ERROR;
    
    if (parse_request_line(headers_view.substr(0, line_end)) == Status::ERROR) {
        return Status::ERROR;
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
    return Status::OK;
}

Status HTTPParser::parse_request_line(std::string_view line) {
    size_t first_space = line.find(' ');
    if (first_space == std::string_view::npos) return Status::ERROR;
    
    size_t second_space = line.find(' ', first_space + 1);
    if (second_space == std::string_view::npos) return Status::ERROR;
    
    request->method = std::string(line.substr(0, first_space));
    request->url = std::string(line.substr(first_space + 1, second_space - (first_space + 1)));
    request->version = std::string(line.substr(second_space + 1));
    std::string_view version_view(request->version);

    if (request->method.empty() || request->url.empty() || !version_view.starts_with("HTTP/")) {
        return Status::ERROR;
    }
    return Status::OK;
}

Status HTTPParser::validate_headers() {
    const auto& headers_map = request->headers->parsed_headers;

    // HTTP/1.1 strictly requires host
    if (request->version == "HTTP/1.1") {
        if (headers_map.find("Host") == headers_map.end()) {
            return Status::ERROR; // Should trigger a 400 Bad Request
        }
    }

    // HTTP Request Smuggling
    // request cannot have both Content-Length and Transfer-Encoding
    bool has_content_length = headers_map.find("Content-Length") != headers_map.end();
    bool has_transfer_encoding = headers_map.find("Transfer-Encoding") != headers_map.end();

    if (has_content_length && has_transfer_encoding) {
        return Status::ERROR; // http code 400
    }

    return Status::OK;
}

Status HTTPParser::process_headers() {
    request->body = std::make_unique<Body>();
    
    auto it = request->headers->parsed_headers.find("Content-Length");
    if (it != request->headers->parsed_headers.end()) {
        try {
            request->body->content_length = std::stoull(it->second);
        } catch (...) {
            return Status::ERROR; // malformed length error
        }
    } else {
        request->body->content_length = 0; // e.g., GET requests
    }
    
    return Status::OK;
}

Status HTTPParser::read_body(const uint8_t*& buf, size_t& length) {
    auto& body = request->body;
    
    // how many bytes of body still remaining
    size_t needed = body->content_length - body->body_length;
    
    // copy what is received and needed i.e. not more than content-length
    size_t to_copy = std::min(length, needed);

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
        std::memcpy(body->frames[current_frame_idx] + offset_in_frame, buf, chunk_size);

        // Advance all pointers and counters
        buf += chunk_size;
        length -= chunk_size;
        to_copy -= chunk_size;
        body->body_length += chunk_size;
    }

    return Status::OK; 
}