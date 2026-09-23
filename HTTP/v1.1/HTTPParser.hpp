#pragma once

#include <string>
#include <string_view>
#include <memory>

#include "Protocol/IProtocolParser.hpp"

class Request;
enum Status;

enum ReadingState {
    ReadingHeaders,
    ReadingBody
};

class HTTPParser: public IProtocolParser {
public:
    HTTPParser();
    ~HTTPParser() = default;

    Status consume(const uint8_t* buffer, size_t length) override;

private:
    Status read_headers(const uint8_t*& buf, size_t& length);
    Status parse_headers(std::string_view headers_view);
    Status parse_request_line(std::string_view line);
    Status process_headers();
    Status validate_headers();
    
    Status read_body(const uint8_t*& buf, size_t& length);
    void reset_state();

    ReadingState state;
    std::unique_ptr<Request> request; // uniue_ptr to avoid overhead of atomic count with shared_ptr
};