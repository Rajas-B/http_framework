#pragma once

#include <string>
#include <string_view>
#include <memory>

#include "http/Protocol/IProtocolParser.hpp"
#include "http/Router/Router.hpp"

class Request;
enum ReadingCodes;

enum ReadingState {
    ReadingHeaders,
    ReadingBody
};

class HTTPParser: public IProtocolParser {
public:
    HTTPParser();
    ~HTTPParser() = default;

    ReadingCodes consume(const uint8_t* buffer, size_t length, size_t& consumed) override;
    std::unique_ptr<Request> return_complete_request();

private:
    ReadingCodes read_headers(const uint8_t* buf, size_t length, size_t& consumed);
    ReadingCodes parse_headers(std::string_view headers_view);
    ReadingCodes parse_request_line(std::string_view line);
    ReadingCodes process_headers();
    ReadingCodes validate_headers();
    
    ReadingCodes read_body(const uint8_t* buf, size_t length, size_t& consumed);
    void reset_state();

    ReadingState state;
    std::unique_ptr<Request> request; // uniue_ptr to avoid overhead of atomic count with shared_ptr
};