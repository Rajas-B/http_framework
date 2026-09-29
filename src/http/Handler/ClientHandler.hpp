#pragma once
#include <atomic>
#include <mutex>
#include <deque>
#include <vector>
#include <cstdint>

#include "reactor/EventHandler.hpp"
#include "http/Parser/HTTPParser.hpp"

class Request;
class EventContext;
class Router;

class ClientHandler: public EventHandler {
public:
    int getfd() override;
    void handle_read() override;
    void handle_write() override;
    void handle_close() override;
    void enqueue_response_and_wake(std::vector<uint8_t> response_bytes);
    ClientHandler(int clientfd, std::unique_ptr<EventContext> ctx, Router& router);
    bool ready_for_write() override;
    void mark_as_processed();
    void send_completed_request(std::unique_ptr<Request>& req);
private:
    int clientfd;
    int wakeup_fd;
    uint8_t buf[8*1024];
    HTTPParser parser;
    std::mutex write_guard;
    std::deque<std::vector<uint8_t>> write_queue;
    std::atomic<bool> is_being_processed{false}; // set to true when this handler is added in reactor's queue, to avoid duplicate entries
    std::unique_ptr<EventContext> ctx;
    Router& router;
};