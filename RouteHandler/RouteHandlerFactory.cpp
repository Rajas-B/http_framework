#pragma once
#include "RouteHandler/RouteHandlerFactory.hpp"

std::unique_ptr<BaseRouteHandler> RouteHandlerFactory::getRouteHandler(HTTPTask task) {
    auto it = routes.find(task.request->url);
    
    if (it != routes.end()) {
        return it->second(std::move(task));
    }
    return std::make_unique<BaseRouteHandler>(std::move(task));
}