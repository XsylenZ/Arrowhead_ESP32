cmake_minimum_required(VERSION 3.2)
project(Arrowhead_ESP32)

if(${COVERAGE})
    set(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)