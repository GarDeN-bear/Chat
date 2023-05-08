cmake_minimum_required(VERSION 3.20)

project(Server)
link_libraries(ws2_32 wsock32)

add_executable(Server Server.cpp)