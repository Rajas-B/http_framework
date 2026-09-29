#include "http/Routes/HomePage.hpp"
#include "http/Data/Response.hpp"

void HomePage::handle_get(Response& res) {
    res.status_code = 200;
    res.body = "Welcome to Rajas's server runtime";
}