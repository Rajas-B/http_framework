cmake_minimum_required(VERSION 3.15)
project(HTTPFramework VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(http_lib
    src/http/Server.cpp
    src/http/parser/HttpParser.cpp
    src/http/router/BaseRouter.cpp
)

target_include_directories(http_lib 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_subdirectory(server_runtime)
target_link_libraries(protocol_app PRIVATE reactor_lib)