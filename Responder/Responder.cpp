#include "Responder/Responder.hpp"

void Responder::send(Response res) {
    if (handler == nullptr || client_fd < 0) {
        return; // Safety check in case this is a disconnected/invalid responder
    }

    std::string serialized_str = res.serialize();

    // 2. Convert the string to a vector of unsigned 8-bit integers.
    // The vector constructor takes the start and end iterators of the string,
    // efficiently copying the underlying bytes over.
    std::vector<uint8_t> response_bytes(serialized_str.begin(), serialized_str.end());

    // 3. Hand the bytes over to the Reactor's outbound queue
    handler->enqueue_response_and_wake(std::move(response_bytes));
}