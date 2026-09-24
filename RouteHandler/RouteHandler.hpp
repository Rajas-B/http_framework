#pragma once

class Request;
class Response;

class RouteHandler {
    virtual void get(const Request& req, Response& res) = 0;
    virtual void post(const Request& req, Response& res) = 0;
    virtual void http_delete(const Request& req, Response& res) = 0;
    virtual void options(const Request& req, Response& res) = 0;
    virtual void put(const Request& req, Response& res) = 0;
};
