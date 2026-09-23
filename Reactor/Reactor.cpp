#pragma once
#include <sys/epoll.h>
#include <unistd.h>

#include "Reactor.hpp"

#include "EventHandler/EventHandler.hpp"
#include "WakeupHandler/WakeupHandler.hpp"
#include "Handler/ClientHandler.hpp"

int Reactor::start() {
    // the infinite loop
    struct epoll_event events[64]; // the events (which will be new connection or a read event)

    while (true) {
        // block till atleast one socket has an event;
        int num_events = epoll_wait(epoll_fd, events, 64, -1); // no timeout
        
        for (int i = 0; i < num_events; ++i) {
            EventHandler* handler = static_cast<EventHandler*>(events[i].data.ptr);
            if (events[i].events & EPOLLIN) {
                handler->handle_read();
            }
            if (events[i].events & EPOLLOUT) {
                handler->handle_write();
            }
            if (events[i].events & EPOLLERR) {
                handler->handle_close();
            }
        }
    }
    return 0;
}

void Reactor::add_handler(EventHandler* handler) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = handler;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, handler->getfd(), &ev);
}

void Reactor::set_wakeup_handler(WakeupHandler* ref_wakeup_handler) {
    wakeup_handler = ref_wakeup_handler;
    add_handler(wakeup_handler);
}

// called by 
void Reactor::add_to_write_ready(ClientHandler* handler) {
    {
        std::lock_guard<std::mutex> lock(write_guard);
        ready_to_write_clients.push(handler);
    }

    // wakeup signal
    wakeup_handler->handle_write();
}

// called by wakeup handler
void Reactor::process_write_clients() {
    std::queue<ClientHandler*> local_queue;
    {
        std::lock_guard<std::mutex> lock(write_guard);
        std::swap(ready_to_write_clients, local_queue);
    }
    while (!local_queue.empty()) {
        EventHandler* handler = local_queue.front();
        local_queue.pop();

        handler->mark_as_processed();
        handler->handle_write();
    }
}

int Reactor::get_epoll_fd() {
    return epoll_fd;
}

void Reactor::modify_epoll(int events, EventHandler* handler) {
    struct epoll_event ev;
    ev.data.ptr = handler;
    ev.events = events;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, handler->getfd(), &ev);
}