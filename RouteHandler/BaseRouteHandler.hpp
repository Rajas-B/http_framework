#pragma once

#include "Data/Request.hpp"
#include "Responder/Responder.hpp"

class HTTPTask;
class Response;

class BaseRouteHandler {
public:
virtual ~BaseRouteHandler() = default;
BaseRouteHandler(HTTPTask task);
void process_request_and_return_response();
protected:
    virtual void handle_get(const Request& req, Response& res);
    virtual void handle_post(const Request& req, Response& res);
    virtual void handle_delete(const Request& req, Response& res);
    virtual void handle_options(const Request& req, Response& res);
    virtual void handle_put(const Request& req, Response& res);  
    Request req;
    Responder responder;
};