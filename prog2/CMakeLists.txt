cmake_minimum_required(VERSION 3.7.2)
project(prog2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE RelWithDebInfo)

include(Warnings.cmake)

include_directories(MWPM/blossom5-v2.03.src)
include_directories(MWPM/blossom5-v2.03.src/GEOM)
include_directories(MWPM/blossom5-v2.03.src/MinCost)

add_executable(prog2
        graph.hpp
        graph.cpp
        MWPM/blossom5-v2.03.src/GEOM/GeomPerfectMatching.h
        MWPM/blossom5-v2.03.src/GEOM/GPMinit.cpp
        MWPM/blossom5-v2.03.src/GEOM/GPMinterface.cpp
        MWPM/blossom5-v2.03.src/GEOM/GPMkdtree.cpp
        MWPM/blossom5-v2.03.src/GEOM/GPMkdtree.h
        MWPM/blossom5-v2.03.src/GEOM/GPMmain.cpp
        MWPM/blossom5-v2.03.src/MinCost/MinCost.cpp
        MWPM/blossom5-v2.03.src/MinCost/MinCost.h
        MWPM/blossom5-v2.03.src/block.h
        MWPM/blossom5-v2.03.src/LCA.h
        MWPM/blossom5-v2.03.src/misc.cpp
        MWPM/blossom5-v2.03.src/PerfectMatching.h
        MWPM/blossom5-v2.03.src/PMduals.cpp
        MWPM/blossom5-v2.03.src/PMexpand.cpp
        MWPM/blossom5-v2.03.src/PMimplementation.h
        MWPM/blossom5-v2.03.src/PMinit.cpp
        MWPM/blossom5-v2.03.src/PMinterface.cpp
        MWPM/blossom5-v2.03.src/PMmain.cpp
        MWPM/blossom5-v2.03.src/PMrepair.cpp
        MWPM/blossom5-v2.03.src/PMshrink.cpp
        MWPM/blossom5-v2.03.src/PQ.h
        MWPM/blossom5-v2.03.src/timer.h
        tjoin.hpp mwc.hpp mwc.cpp tjoin.cpp main.cpp )
