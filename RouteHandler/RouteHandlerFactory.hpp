#pragma once
#include <unordered_map>
#include <functional>
#include <memory>

#include "RouteHandler/BaseRouteHandler.hpp"

class RouteHandlerFactory {
public:
    using HandlerFactory = std::function<std::unique_ptr<BaseRouteHandler>(HTTPTask)>;
    std::unique_ptr<BaseRouteHandler> getRouteHandler(HTTPTask task);
    
    void register_route(const std::string& path, HandlerFactory factory) {
        routes[path] = std::move(factory);
    }
    
private:
    std::unordered_map<std::string, HandlerFactory> routes;
};