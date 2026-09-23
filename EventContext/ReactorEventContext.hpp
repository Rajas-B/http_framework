#include "EventContext.hpp"

class EventHandler;
class Reactor;

class ReactorEventContext: public EventContext {
public:
    void request_write_wakeup() override;
    void remove_write_wakeup() override;
    void add_to_write_queue() override;
    void bind_handler(EventHandler* client_handler) override;
private:
    Reactor& reactor;
    EventHandler* handler;
};