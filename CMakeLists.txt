cmake_minimum_required(VERSION 4.2)
project(cave)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(cave
        cave.cpp
        cave.h
        grader.cpp)
