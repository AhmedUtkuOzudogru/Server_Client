cmake_minimum_required(VERSION 3.28)
project(Server_Client)

set(CMAKE_CXX_STANDARD 17)

add_executable(Server_Client main.cpp
        Server.cpp
        Server.h
        Client.cpp
        Client.h)
target_link_libraries(Server_Client ws2_32)