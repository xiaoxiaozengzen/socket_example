cmake_minimum_required(VERSION 3.23)
project(tcp_example)

add_executable(tcp_client tcp_client.cpp)
add_executable(tcp_server tcp_server.cpp)
add_executable(second_tcp_client second_tcp_client.cpp)