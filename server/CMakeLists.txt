cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(se3313_lab4_server)

set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

include(./server/server.cmake)

