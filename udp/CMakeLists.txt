cmake_minimum_required(VERSION 3.23)
project(udp_example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(udp_server udp_server.cpp)
add_executable(udp_client udp_client.cpp)