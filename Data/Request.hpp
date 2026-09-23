#pragma once
#include <string>
#include <memory>
#include "Headers.hpp"
#include "Body.hpp"

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