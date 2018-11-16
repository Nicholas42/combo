cmake_minimum_required(VERSION 3.5.1)
project(prog1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)
include(Warnings.cmake)

include_directories(.)

add_executable(edmonds.out
        graph.cpp
        graph.hpp
        main.cpp edmonds.cpp edmonds.hpp)
