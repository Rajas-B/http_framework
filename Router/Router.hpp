// not sure yet who will call dispatch()
// but dispatch will 
#pragma once
#include "ThreadPool/ThreadPool.hpp"
#include "Tasks/HTTPTask.hpp"
#include "RouteHandlerFactory.hpp"

class Router {
public:
    Router(const int workers);
    void dispatch(HTTPTask task);
private:
// thread pool
    ThreadPool* thread_pool; // need to give number of threads
    RouteHandlerFactory route_factory; // entry point for actual application logic
// request queue
// mapper

};