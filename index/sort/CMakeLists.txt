cmake_minimum_required (VERSION 2.6)

set (sort sort)

set (CMAKE_BUILD_TYPE "Release")
set (CMAKE_CXX_FLAGS "-std=c++11 -O3 -Wall -pthread")

set (sources
    main.cpp
    ../util/threadpool.cpp)

include_directories(../util)

add_executable(${sort} ${sources})
