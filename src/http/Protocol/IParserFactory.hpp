#pragma once
#include "http/Protocol/IProtocolParser.hpp"

class IParserFactory {
public:
    virtual IProtocolParser* createParser() = 0;
};
