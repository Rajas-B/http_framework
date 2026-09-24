#pragma once

#include "EventHandler/EventHandler.hpp"
#include "Reactor/Reactor.hpp"
#include "EventContext/EventContext.hpp"
#include "EventContext/ReactorEventContext.hpp"
#include <atomic>
#include <mutex>
#include <deque>
#include <vector>
#include <cstdint>

#include <memory>
#include <sys/socket.h>

template <typename THandler>
class Acceptor: public EventHandler {
public:
    int getfd () override {
        return server_fd;
    }
    Acceptor(int server_fd, Reactor* reactor, int wakeup_fd): 
    server_fd(server_fd), reactor(reactor), wakeup_fd(wakeup_fd) {}

    void handle_read() override {
        int clientfd = accept(server_fd, nullptr, nullptr);
        
        // added client handler
        std::unique_ptr<EventContext> context = std::make_unique<ReactorEventContext>(*reactor);
        EventHandler* handler = new THandler(clientfd, std::move(context));
        reactor->add_handler(handler);
    }
private:
    int server_fd;
    int wakeup_fd;
    Reactor* reactor;
};