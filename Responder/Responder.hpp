#include "Handler/ClientHandler.hpp"
#include "Data/Response.hpp"

class Responder {
public:
    Responder(int fd, ClientHandler* handler): client_fd(fd), handler(handler) {}
    void send(Response res);
private:
    int client_fd;
    ClientHandler* handler;
};