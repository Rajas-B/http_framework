#pragma once

class ClientHandler;
class Response;

class Responder {
public:
    Responder(int fd, ClientHandler* handler): client_fd(fd), handler(handler) {}
    void send(Response res);
private:
    int client_fd;
    ClientHandler* handler;
};