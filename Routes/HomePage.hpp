#pragma once
#include "RouteHandler/BaseRouteHandler.hpp"

class HomePage: public BaseRouteHandler {
    void handle_get(const Request& req, Response& res);
};