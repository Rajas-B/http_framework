// wrapper around request task
// so that request doesn't have to manage sending response
#pragma once
#include <memory>

class Request;
class Responder;

struct HTTPTask {
    std::unique_ptr<Request> request;
    std::unique_ptr<Responder> responder;

    HTTPTask(std::unique_ptr<Request> req, std::unique_ptr<Responder> res): 
        request(std::move(req)), responder(std::move(res)) {}
};