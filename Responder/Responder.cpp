#include "Responder/Responder.hpp"

#include "Handler/ClientHandler.hpp"
#include "Data/Response.hpp"

void Responder::send(Response res) {
    if (handler == nullptr || client_fd < 0) {
        return;
    }
    std::vector<uint8_t> response_bytes = res.serialize();

    handler->enqueue_response_and_wake(std::move(response_bytes));
}