class EventHandler {
public:
    virtual ~EventHandler() = default; 

    virtual int getfd() = 0;
    virtual void handle_read() = 0;
    virtual void handle_write() {}
    virtual void handle_close() {}
    virtual bool ready_for_write() { return false; }
    virtual void mark_as_processed() {}
};