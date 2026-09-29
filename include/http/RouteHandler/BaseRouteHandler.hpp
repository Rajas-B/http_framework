#pragma once

#include <memory>

class HTTPTask;
class Response;
class Request;
class Responder;


class BaseRouteHandler {
public:
virtual ~BaseRouteHandler() = default;
BaseRouteHandler(HTTPTask&& task);
void process_request_and_return_response();
protected:
    virtual void handle_get(Response& res);
    virtual void handle_post(Response& res);
    virtual void handle_delete(Response& res);
    virtual void handle_options(Response& res);
    virtual void handle_put(Response& res);  
    std::unique_ptr<Request> req;
    std::unique_ptr<Responder> responder;
};