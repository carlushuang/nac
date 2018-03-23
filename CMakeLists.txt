cmake_minimum_required(VERSION 3.0)

file(STRINGS "VERSION" nac_VERSION)
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" nac_VERSION_MAJOR "${nac_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" nac_VERSION_MINOR "${nac_VERSION}")

# CMP0048    https://cmake.org/cmake/help/v3.0/policy/CMP0048.html
# project(nac VERSION ${nac_VERSION})
if(POLICY CMP0048)
    cmake_policy(SET CMP0048 OLD)   # cmake 3.0+
endif()
#project(nac LANGUAGES "CXX")
project(nac)

# Check if we build standalone, or included by other cmake project
set(nac_BUILD_STANDALONE OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(nac_BUILD_STANDALONE ON)
endif()

# @option: BUILD_TEST, bool
# @option: BUILD_INSTALL, string for install path
option(BUILD_TEST "build test" ${nac_BUILD_STANDALONE})
option(BUILD_INSTALL "build for install" ${nac_BUILD_STANDALONE})

# @option: BUILD_ARMV7, bool
# @option: BUILD_ARMV7_AS_PLUGIN, bool
option(BUILD_ARMV7 "need build armv7-a ops, only valid when cross compile" TRUE)
option(BUILD_ARMV7_AS_PLUGIN "weather build armv7-a ops as plugin, aka, sharelib" TRUE)

macro(nac_hw_plugin_target target_name hw_name)
    set(${target_name} nac.hw.${hw_name})
endmacro()

# default build type
#if(NOT CMAKE_BUILD_TYPE)
#    set(CMAKE_BUILD_TYPE Release)
#endif()

# https://public.kitware.com/pipermail/cmake/2010-December/041135.html
# default install path
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "default install path" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)


if(BUILD_ARMV7)
    set(_nac_ARMV7_SRC)
endif()


# main project
set(_nac_SRC)
set(_nac_LOCAL_INCLUDE_DIR)
set(_nac_EXPORT_INCLUDE_DIR)
set(_nac_EXPORT_HEADER)
add_subdirectory(nac)
if(BUILD_ARMV7 AND NOT BUILD_ARMV7_AS_PLUGIN)
    set(_nac_SRC ${_nac_SRC} ${_nac_ARMV7_SRC})
elseif(BUILD_ARMV7 AND BUILD_ARMV7_AS_PLUGIN)
    #set(_nac_SRC ${_nac_SRC} ${_nac_ARMV7_SRC})
endif()



add_library(nac_obj OBJECT ${_nac_SRC})
set_property(TARGET nac_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

target_include_directories(nac_obj PRIVATE ${_nac_LOCAL_INCLUDE_DIR})
target_include_directories(nac_obj PUBLIC ${_nac_EXPORT_INCLUDE_DIR})


#set(_nac_OPT -Os -ffast-math)
set(_nac_OPT -g)
set(_nac_CXX_FLAGS ${_nac_OPT} -std=c++11 -Wall -Wno-switch -Wno-reorder -Wno-sign-compare)


#if(NOT MSVC)

target_compile_options(nac_obj PRIVATE ${_nac_CXX_FLAGS})
    #if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

#endif()

add_library(nac SHARED $<TARGET_OBJECTS:nac_obj>)
add_library(nac::nac ALIAS nac)

add_library(nac-static STATIC $<TARGET_OBJECTS:nac_obj>)
add_library(nac::nac-static ALIAS nac-static)

# thread lib for c++11 std::thread related
if(CMAKE_VERSION VERSION_GREATER 3.1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(nac Threads::Threads)
    target_link_libraries(nac-static Threads::Threads)
else()
    find_package(Threads REQUIRED)
    if(THREADS_HAVE_PTHREAD_ARG)
        target_compile_options(nac PUBLIC "-pthread")
        target_compile_options(nac-static PUBLIC "-pthread")
    endif()
    if(CMAKE_THREAD_LIBS_INIT)
        target_link_libraries(nac "${CMAKE_THREAD_LIBS_INIT}")
        target_link_libraries(nac-static "${CMAKE_THREAD_LIBS_INIT}")
    endif()
endif()

# relative path afer install prefix
set(_install_header_dir include)
set(_install_lib_dir lib)
set(_install_cfg_dir share/cmake/nac)
# target to be imported by outside project
set(_export_targets_name nac-targets)

# manually set public header for target file
target_include_directories(nac INTERFACE $<INSTALL_INTERFACE:${_install_header_dir}>
                                        $<BUILD_INTERFACE:${_nac_EXPORT_INCLUDE_DIR}>)
target_include_directories(nac-static INTERFACE $<INSTALL_INTERFACE:${_install_header_dir}>
                                        $<BUILD_INTERFACE:${_nac_EXPORT_INCLUDE_DIR}>)
target_compile_options(nac-static INTERFACE ${_nac_CXX_FLAGS})

# for use of configure_package_config_file()
include(CMakePackageConfigHelpers)

if(BUILD_ARMV7 AND BUILD_ARMV7_AS_PLUGIN)
    nac_hw_plugin_target(nac_armv7_target armv7)
    #message(STATUS "nac_armv7_target:${nac_armv7_target}")
    add_library(${nac_armv7_target} SHARED ${_nac_ARMV7_SRC})
    target_include_directories(${nac_armv7_target} PRIVATE ${_nac_LOCAL_INCLUDE_DIR})
    target_include_directories(${nac_armv7_target} PRIVATE ${_nac_EXPORT_INCLUDE_DIR})
endif()

if(BUILD_INSTALL)
    install(TARGETS nac EXPORT ${_export_targets_name} LIBRARY DESTINATION ${_install_lib_dir})
    install(TARGETS nac-static EXPORT ${_export_targets_name} ARCHIVE DESTINATION ${_install_lib_dir})
    install(FILES ${_nac_EXPORT_HEADER} DESTINATION ${_install_header_dir})
    # config files
    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/nac-config.cmake.in
                                   ${CMAKE_CURRENT_BINARY_DIR}/${_install_cfg_dir}/nac-config.cmake
                                   INSTALL_DESTINATION ${_install_cfg_dir})

    write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${_install_cfg_dir}/nac-config-version.cmake
                                   VERSION ${${PROJECT_NAME}_VERSION} COMPATIBILITY AnyNewerVersion)

    install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/${_install_cfg_dir}/nac-config.cmake
                    ${CMAKE_CURRENT_BINARY_DIR}/${_install_cfg_dir}/nac-config-version.cmake
                    DESTINATION ${_install_cfg_dir})

    # export target to file
    install(EXPORT ${_export_targets_name} NAMESPACE "nac::" DESTINATION  ${_install_cfg_dir})

    # plugin for compute device
    if(BUILD_ARMV7 AND BUILD_ARMV7_AS_PLUGIN)
        install(TARGETS ${nac_armv7_target} LIBRARY DESTINATION ${_install_lib_dir})
    endif()
endif()

if(BUILD_TEST)
    add_subdirectory(nac/test)
endif()


message(STATUS "nac_VERSION: ${nac_VERSION}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
if(CMAKE_CROSSCOMPILING)
    message(STATUS "CROSS_COMPILE_PREFIX=${CROSS_COMPILE_PREFIX}")
endif()