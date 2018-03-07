cmake_minimum_required(VERSION 2.8.8)

project(nac)
file(STRINGS "VERSION" nac_VERSION)
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" nac_VERSION_MAJOR "${nac_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" nac_VERSION_MINOR "${nac_VERSION}")

option(BUILD_TEST "build test" OFF)

# default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# https://public.kitware.com/pipermail/cmake/2010-December/041135.html
# default install path
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "default install path" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

add_subdirectory(nac)

message(STATUS "nac_VERSION: ${nac_VERSION}")
# message(STATUS "nac_VERSION_MAJOR: ${nac_VERSION_MAJOR}")
# message(STATUS "nac_VERSION_MINOR: ${nac_VERSION_MINOR}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
# message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
