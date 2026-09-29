#pragma once
#include "Router/Router.hpp"
#include "http/Tasks/HTTPTask.hpp"
#include "http/ThreadPool/ThreadPool.hpp"
#include "http/RouteHandler/BaseRouteHandler.hpp"

Router::Router(int workers) {
    thread_pool = new ThreadPool(4);
}

void Router::dispatch(HTTPTask&& task) {
    auto http_task = [this, task=std::move(task)]() mutable {
        auto route = route_factory.getRouteHandler(std::move(task));
        route->process_request_and_return_response();
    };
    thread_pool->enqueue(std::move(http_task));
}

