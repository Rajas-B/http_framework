#include "RouteHandler/BaseRouteHandler.hpp"

#include <utility>

#include "Data/Response.hpp"
#include "Tasks/HTTPTask.hpp"

BaseRouteHandler::BaseRouteHandler(HTTPTask t) 
    : req(std::move(*t.request)),
      responder(std::move(t.responder)) 
{}

void BaseRouteHandler::process_request_and_return_response() {
    Response res;
    if (req.method == "GET") {
        handle_get(req, res);
    } 
    else if (req.method == "POST") {
        handle_post(req, res);
    } 
    else if (req.method == "PUT") {
        handle_put(req, res);
    }
    else if (req.method == "DELETE") {
        handle_delete(req, res);
    }
    else {
        res.status_code = 405; // Method Not Allowed
        res.body = "Method Not Allowed";
    }

    // Safely dispatch the populated response back to the Reactor's outbound queues
    responder.send(std::move(res));
}

void BaseRouteHandler::handle_get(const Request& req, Response& res) {
    res.status_code = 404;
    res.body = "Not Found";
}

void BaseRouteHandler::handle_post(const Request& req, Response& res) {
    res.status_code = 404;
    res.body = "Not Found";
}

void BaseRouteHandler::handle_put(const Request& req, Response& res) {
    res.status_code = 404;
    res.body = "Not Found";
}

void BaseRouteHandler::handle_delete(const Request& req, Response& res) {
    res.status_code = 404;
    res.body = "Not Found";
}