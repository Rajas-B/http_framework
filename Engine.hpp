#pragma once

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/eventfd.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <memory>

#include "Reactor/Reactor.hpp"
#include "Handler/ClientHandler.hpp"
#include "WakeupHandler/WakeupHandler.hpp"
#include "Acceptor/Acceptor.hpp"

// this is the wrapper class around the entire application
// for now I have not separated the runtime
// technically a runtime's boundary lies at where client handler sends the request to the Router

// setting the boundary between runtime and protocol engine
class Engine { 
public:
    int setup_listening_fd(const bool blocking);
    void bind_to_port(const int port, const int& server_fd);
    int setup_event_fd();
    void start();
private:
    int server_fd;
    int wakeup_fd;
};