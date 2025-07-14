cmake_minimum_required(VERSION 3.23)
project(uds_example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(uds_example_client uds_example_client.cpp)
add_executable(uds_example_server uds_example_server.cpp)