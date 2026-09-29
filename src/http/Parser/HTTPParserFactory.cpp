#pragma once
#include "http/Parser/HTTPParserFactory.hpp"

HTTPParser* HTTPParserFactory::createParser() {
    return new HTTPParser();
}