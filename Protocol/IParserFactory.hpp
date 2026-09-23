#pragma once
#include "IProtocolParser.hpp"

class IParserFactory {
public:
    virtual IProtocolParser* createParser() = 0;
};
