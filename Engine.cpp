#pragma once

#include "Engine.hpp"

int Engine::setup_listening_fd(const bool blocking) {
    int fd = socket(AF_INET, SOCK_STREAM | (blocking?0:SOCK_NONBLOCK), 0);
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    server_fd = fd;
    return server_fd;
}

void Engine::bind_to_port(const int port, const int& server_fd) {
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, SOMAXCONN);
}

// for wakeup handler
int Engine::setup_event_fd() {
    // 0 is the initial counter value
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd == -1) {
        throw std::runtime_error("Failed to create eventfd");
    }
    wakeup_fd = fd;
    return wakeup_fd;
}

void Engine::start() {
    auto reactor = new Reactor(8080, wakeup_fd);

    // one acceptor fd to accept new client connections
    EventHandler* acceptor = new Acceptor<ClientHandler>(server_fd, reactor, wakeup_fd);
    reactor->add_handler(acceptor);

    WakeupHandler wakeupHandler(wakeup_fd, reactor);
    reactor->set_wakeup_handler(&wakeupHandler);
    reactor->add_handler(&wakeupHandler);

    reactor->start();
}


int main() {
    // create runtime, as a static object?
    // start it
    
    // set up non blocking listening socket
    int server_fd = setup_listening_fd(false);
    // create an event fd
    int wakeup_fd = setup_event_fd();
    
    // bind to port 8080
    bind_to_port(8080, server_fd);

    // create epoll instance in OS
    // which is managed by the reactorrrr
    auto reactor = new Reactor(8080, wakeup_fd);

    // one acceptor fd to accept new client connections
    EventHandler* acceptor = new Acceptor<ClientHandler>(server_fd, reactor, wakeup_fd);
    reactor->add_handler(acceptor);

    WakeupHandler wakeupHandler(wakeup_fd, reactor);
    reactor->set_wakeup_handler(&wakeupHandler);
    reactor->add_handler(&wakeupHandler);

    reactor->start();
}