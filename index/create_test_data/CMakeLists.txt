cmake_minimum_required (VERSION 2.6)

set (create_test_data create_test_data)

set (CMAKE_BUILD_TYPE "Release")
set (CMAKE_CXX_FLAGS "-std=c++11 -O3 -Wall")

set (sources
    main.cpp)

include_directories(../util)

add_executable(${create_test_data} ${sources})
