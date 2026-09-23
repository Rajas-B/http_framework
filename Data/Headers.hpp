#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

class Headers {
public:
    static constexpr size_t k_max_header_size = 8192;
    
    size_t bytes_accumulated = 0;
    std::unordered_map<std::string, std::string> parsed_headers;
    uint8_t headers_buf[k_max_header_size];
};