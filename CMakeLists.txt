cmake_minimum_required(VERSION 3.0)

file(STRINGS "VERSION" nac_VERSION)
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" nac_VERSION_MAJOR "${nac_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" nac_VERSION_MINOR "${nac_VERSION}")

# CMP0048    https://cmake.org/cmake/help/v3.0/policy/CMP0048.html
# project(nac VERSION ${nac_VERSION})
if(POLICY CMP0048)
    cmake_policy(SET CMP0048 OLD)   # cmake 3.0+
endif()
project(nac LANGUAGES "CXX")

# options
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

# main project
add_subdirectory(nac)

# config files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/nac-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/nac-config.cmake @ONLY)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/nac-config-version.cmake.in
#        ${CMAKE_CURRENT_BINARY_DIR}/cmake/nac-config-version.cmake @ONLY)
#install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/cmake/nac-config.cmake
#                ${CMAKE_CURRENT_BINARY_DIR}/cmake/nac-config-version.cmake
#                DESTINATION share/cmake/nac)
install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/cmake/nac-config.cmake
                 DESTINATION share/cmake/nac)

message(STATUS "nac_VERSION: ${nac_VERSION}")
# message(STATUS "nac_VERSION_MAJOR: ${nac_VERSION_MAJOR}")
# message(STATUS "nac_VERSION_MINOR: ${nac_VERSION_MINOR}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
# message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
