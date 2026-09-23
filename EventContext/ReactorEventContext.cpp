#pragma once
#include "ReactorEventContext.hpp"

#include "Reactor/Reactor.hpp"
#include "EventHandler/EventHandler.hpp"

void ReactorEventContext::bind_handler(EventHandler* client_handler) {
    handler = client_handler;
}

void ReactorEventContext::request_write_wakeup() {
    reactor.modify_epoll(EPOLLIN|EPOLLET|EPOLLOUT, handler);
}

void ReactorEventContext::remove_write_wakeup() {
    reactor.modify_epoll(EPOLLIN|EPOLLET, handler);
}

void ReactorEventContext::add_to_write_queue() {

}