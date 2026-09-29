#pragma once
#include <string>
#include <memory>
#include "http/Data/Headers.hpp"
#include "http/Data/Body.hpp"

class Request {
public:
    std::string method;
    std::string url;
    std::string version;

    std::unique_ptr<Headers> headers;
    std::unique_ptr<Body> body;

    Request() {
        headers = std::make_unique<Headers>();
    }
};