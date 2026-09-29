#pragma once
#include "http/RouteHandler/BaseRouteHandler.hpp"

class HomePage: public BaseRouteHandler {
    void handle_get(Response& res) override;
};