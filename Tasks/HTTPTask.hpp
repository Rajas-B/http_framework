// wrapper around request task
// so that request doesn't have to manage sending response
#pragma once
#include <memory>

#include "Data/Request.hpp"
#include "Responder/Responder.hpp"

struct HTTPTask {
    std::unique_ptr<Request> request;
    Responder responder;
};