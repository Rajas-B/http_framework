#pragma once
#include <atomic>
#include <mutex>
#include <deque>
#include <vector>
#include <cstdint>

#include "EventHandler/EventHandler.hpp"
#include "HTTP/v1.1/HTTPParser.hpp"

class EventContext;

class ClientHandler: public EventHandler {
public:
    int getfd() override;
    void handle_read() override;
    void handle_write() override;
    void handle_close() override;
    void enqueue_response_and_wake(std::vector<uint8_t> response_bytes);
    ClientHandler(int clientfd, std::unique_ptr<EventContext> ctx);
    bool ready_for_write() override;
    void mark_as_processed();
private:
    int clientfd;
    int wakeup_fd;
    uint8_t buf[8*1024];
    HTTPParser parser;
    std::mutex write_guard;
    std::deque<std::vector<uint8_t>> write_queue;
    std::atomic<bool> is_being_processed{false}; // set to true when this handler is added in reactor's queue, to avoid duplicate entries
    std::unique_ptr<EventContext> ctx;
};