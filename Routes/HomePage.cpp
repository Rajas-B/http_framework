#include "Routes/HomePage.hpp"

void HomePage::handle_get(const Request& req, Response& res) {
    res.status_code = 200;
    res.body = "Welcome to Rajas's server runtime";
}