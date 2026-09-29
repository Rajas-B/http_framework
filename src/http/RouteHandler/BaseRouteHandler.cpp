#include "http/RouteHandler/BaseRouteHandler.hpp"

#include <utility>

#include "http/Data/Response.hpp"
#include "http/Tasks/HTTPTask.hpp"
#include "http/Data/Request.hpp"
#include "http/Responder/Responder.hpp"

BaseRouteHandler::BaseRouteHandler(HTTPTask&& t) 
    : req(std::move(t.request)),
      responder(std::move(t.responder)) 
{}

void BaseRouteHandler::process_request_and_return_response() {
    Response res;
    if (req->method == "GET") {
        handle_get(res);
    } 
    else if (req->method == "POST") {
        handle_post(res);
    } 
    else if (req->method == "PUT") {
        handle_put(res);
    }
    else if (req->method == "DELETE") {
        handle_delete(res);
    }
    else {
        res.status_code = 405; // Method Not Allowed
        res.body = "Method Not Allowed";
    }

    // Safely dispatch the populated response back to the Reactor's outbound queues
    responder->send(std::move(res));
}

void BaseRouteHandler::handle_get(Response& res) {
    res.status_code = 404;
    res.body = "Not Found";
}

void BaseRouteHandler::handle_post(Response& res) {
    res.status_code = 404;
    res.body = "Not Found";
}

void BaseRouteHandler::handle_put(Response& res) {
    res.status_code = 404;
    res.body = "Not Found";
}

void BaseRouteHandler::handle_options(Response& res) {
    res.status_code = 404;
    res.body = "Not Found";
}

void BaseRouteHandler::handle_delete(Response& res) {
    res.status_code = 404;
    res.body = "Not Found";
}