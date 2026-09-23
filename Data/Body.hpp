#pragma once
#include <vector>
#include <cstdint>

class Body {
public:
    static constexpr size_t k_frame_size = 8192;

    size_t content_length = 0;
    size_t body_length = 0;
    std::vector<uint8_t*> frames;

    Body() = default;
    
    ~Body() {
        for (uint8_t* frame : frames) {
            delete[] frame;
        }
    }
};