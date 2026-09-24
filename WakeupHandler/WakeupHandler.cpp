#include "WakeupHandler.hpp"

#include <cstdint>

#include "Reactor/Reactor.hpp"

int WakeupHandler::getfd() {
    return wakeup_fd;
}

void WakeupHandler::handle_read() {
    // some client has data to read
    // the client has already enqueued itself to the reactor's client queue
    // clear the wakeup signal to avoid infinite looping
    // and then explicitly go and write clients (and other processing; check reactor and ClientHandler)
    uint64_t clear_buffer;
    ssize_t bytes_read = read(wakeup_fd, &clear_buffer, sizeof(uint64_t));
    if (bytes_read == -1 | errno != EAGAIN) {
        // catastrophic failure, not sure what this means yet
    }

    reactor->process_write_clients();
}

// wake up the event poll using this function
void WakeupHandler::handle_write() {
    uint64_t wakeup_signal = 1;
    write(wakeup_fd, &wakeup_signal, sizeof(uint64_t));
}