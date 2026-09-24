#pragma once
#include "ClientHandler.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>

#include "EventContext/EventContext.hpp"

ClientHandler::ClientHandler(int clientfd, std::unique_ptr<EventContext> ctx): 
clientfd(clientfd), ctx(std::move(ctx)) {
    ctx->bind_handler(this);
}

int ClientHandler::getfd() {
    return clientfd;
}

void ClientHandler::handle_read() {
    while (true) {
        ssize_t bytes_read = read(clientfd, buf, 8192);
        if (bytes_read == -1 && errno == EAGAIN){    
            break;
        }
        if (bytes_read == 0) {
            handle_close();
            break;
        }
        // parser consume
        if (parser.consume(buf, bytes_read) == Status::ERROR) {
            break;
        }
    }
}

// will be called once the response is completed
void ClientHandler::enqueue_response_and_wake(std::vector<uint8_t> response_bytes) {
    // thread safe to push response bytes onto queue, to keep responses continuguous
    {
        std::lock_guard<std::mutex> lock(write_guard);
        write_queue.push_back(std::move(response_bytes));
    }
    // create a wakeup signal
    // this will unlock epoll wait because there is an event on the wakeup handler
    if (is_being_processed.exchange(true) == false) {
        ctx->add_to_write_queue();
    }
}

void ClientHandler::handle_write() {
    // we have a queue of messages to write
    // write till the queue is empty
    std::lock_guard<std::mutex> lock(write_guard);

    while(!write_queue.empty()) {
        // take the message at the front of the queue
        auto& message = write_queue.front();
        ssize_t bytes_written = write(clientfd, message.data(), message.size());

        if (bytes_written == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            // the OS buffer is full, can't write now
            break;
        }
        if (bytes_written < message.size()) {
            // partial write occurred
            // update the first message to contain only the remaining bytes
            message.erase(message.begin(), message.begin() + bytes_written);
            // break now since a partial write indicates that the OS buffer might be full
            break;
        }
        else {
            write_queue.pop_front();
        }
    }
    if (!write_queue.empty()) {
        // queue is not empty, so create an EPOLLOUT event
        ctx->remove_write_wakeup();
    }
    else {
        // writing is complete, EPOLLOUT will waste processing time
        // remove it
        ctx->remove_write_wakeup();
    }
}

bool ClientHandler::ready_for_write() {
    return !write_queue.empty();
}

// not in the reactor's queue anymore, called in reactor
void ClientHandler::mark_as_processed() {
    is_being_processed.store(false);
}

void ClientHandler::handle_close() {
    
}