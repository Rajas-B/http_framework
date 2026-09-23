#include "HTTPParserFactory.hpp"

HTTPParser* HTTPParserFactory::createParser() {
    return new HTTPParser();
}