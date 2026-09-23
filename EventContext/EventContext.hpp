class EventHandler;
class EventContext {
public:
    // methods that would be used in Handlers
    virtual void request_write_wakeup() = 0;
    virtual void remove_write_wakeup() = 0;
    virtual void add_to_write_queue() = 0;
    virtual void bind_handler(EventHandler* client_handler) = 0;
};