cmake_minimum_required(VERSION 3.10)
project(prog1)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(edmonds.out
        graph.cpp
        graph.hpp
        main.cpp edmonds.cpp edmonds.h)
