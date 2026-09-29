#pragma once

#include <memory>

#include "http/RouteHandler/RouteHandlerFactory.hpp"
#include "http/Tasks/HTTPTask.hpp"
#include "http/RouteHandler/BaseRouteHandler.hpp"
#include "http/Data/Request.hpp"
#include "http/Responder/Responder.hpp"


std::unique_ptr<BaseRouteHandler> RouteHandlerFactory::getRouteHandler(HTTPTask&& task) {
    auto it = routes.find(task.request->url);
    
    if (it != routes.end()) {
        return it->second(std::move(task));
    }
    return std::make_unique<BaseRouteHandler>(std::move(task));
}