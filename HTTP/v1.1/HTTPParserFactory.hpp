#include "Protocol/IParserFactory.hpp"
#include "HTTPParser.hpp"

class HTTPParserFactory: public IParserFactory {
    public:
        HTTPParser* createParser() override;
};