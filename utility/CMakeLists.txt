cmake_minimum_required(VERSION 3.23)
project(utility_example)

file(GLOB SOURCES "*.cpp")
add_executable(utility_example ${SOURCES})