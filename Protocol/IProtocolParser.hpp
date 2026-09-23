#include <stdint.h>
#include <cstdlib>

#include "Status.hpp"

class IProtocolParser {
public:
    // Returns a status telling the reader what to do next
    virtual Status consume(const uint8_t* buffer, size_t length) = 0;
    virtual ~IProtocolParser() = default;
};