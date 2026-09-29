#pragma once

#include <stdint.h>
#include <cstdlib>

enum ReadingCodes;

class IProtocolParser {
public:
    // Returns a status telling the reader what to do next
    virtual ReadingCodes consume(const uint8_t* buffer, size_t length, size_t& consumed) = 0;
    virtual ~IProtocolParser() = default;
};