cmake_minimum_required(VERSION 2.8)
project(nac)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_subdirectory(nac)

message("CMAKE_BUILD_TYPE:${CMAKE_BUILD_TYPE}")