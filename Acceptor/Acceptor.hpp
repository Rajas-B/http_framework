#pragma once

#include "EventHandler/EventHandler.hpp"


class IParserFactory;
class Reactor;

template <typename THandler>
class Acceptor: public EventHandler {
public:
    int getfd () {
        return server_fd;
    }
    Acceptor(int server_fd, Reactor* reactor, int wakeup_fd): 
    server_fd(server_fd), reactor(reactor), wakeup_fd(wakeup_fd) {}

    void handle_read() {
        int clientfd = accept(server_fd, nullptr, nullptr);
        std::cout << "Accepting new client connection on " << clientfd << std::endl;

        // added client handler
        EventHandler* handler = new THandler(clientfd, );
        reactor->add_handler(handler);
    }

    void handle_write() override;   
    void handle_close() override;
private:
    int server_fd;
    int wakeup_fd;
    Reactor* reactor;
    IParserFactory* parserFactory;
};