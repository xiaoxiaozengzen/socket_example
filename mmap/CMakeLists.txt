cmake_minimum_required(VERSION 3.23)
project(mmap_example)

set(CMAKE_C_STANDARD 99)

add_executable(mmap_example_server mmap_example_server.cpp)
target_link_libraries(mmap_example_server pthread rt)
add_executable(mmap_example_client mmap_example_client.cpp)
target_link_libraries(mmap_example_client pthread rt)

add_executable(sendtofile_example sendtofile_example.cpp)