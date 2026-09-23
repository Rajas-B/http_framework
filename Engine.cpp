#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/eventfd.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <memory>

#include "Logger/Logger.hpp"
#include "Reactor/Reactor.hpp"
#include "Acceptor/Acceptor.hpp"
#include "WakeupHandler/WakeupHandler.hpp"

int setup_listening_fd(const bool blocking) {
    int server_fd = socket(AF_INET, SOCK_STREAM | (blocking?0:SOCK_NONBLOCK), 0);
    int val = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    return server_fd;
}

int bind_to_port(const int port, const int& server_fd) {
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, SOMAXCONN);
}

// for wakeup handler
int setup_event_fd() {
    // o is the initial counter value
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd == -1) {
        throw std::runtime_error("Failed to create eventfd");
    }
    return fd;
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
    EventHandler* acceptor = new Acceptor(server_fd, reactor, wakeup_fd);
    reactor->add_handler(acceptor);

    WakeupHandler wakeupHandler(wakeup_fd, reactor);
    reactor->set_wakeup_handler(&wakeupHandler);
    reactor->add_handler(&wakeupHandler);

    reactor->start();
}