#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <queue>
#include <mutex>

class EventHandler;
class ClientHandler;
class WakeupHandler;

class Reactor {
public:
    Reactor(const int& port, int wakeup_fd): port(port) {
        epoll_fd = epoll_create1(0);
    }
    void add_handler(EventHandler* handler);
    void add_to_write_ready(ClientHandler* handler);
    void set_wakeup_handler(WakeupHandler* wakeup_handler);
    void process_write_clients();
    int get_epoll_fd();
    int start();
    void modify_epoll(int events, EventHandler* handler);
private:
    // this is the epoll fd
    int epoll_fd;
    WakeupHandler* wakeup_handler;
    uint port;
    std::queue<ClientHandler*> ready_to_write_clients;
    std::mutex write_guard;
};

