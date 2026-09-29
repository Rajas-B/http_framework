#pragma once

#include "http/Protocol/IParserFactory.hpp"
#include "http/Parser/HTTPParser.hpp"

class HTTPParserFactory: public IParserFactory {
    public:
        HTTPParser* createParser() override;
};