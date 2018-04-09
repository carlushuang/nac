#
# ARM_ABI
#

if( CMAKE_CROSSCOMPILING )
    # in case re-enter toolchain file
    return()
endif()

# check following flag for support compiling this target
set(VALID_FOR_BUILD_ARMV7 false CACHE BOOL "" FORCE)

#set(GCC_COMPILER_VERSION "" CACHE STRING "GCC Compiler version")
set(CROSS_COMPILE_PREFIX "arm-linux-gnueabihf" CACHE STRING "GNU compiler triple")

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)     # maybe kernel version, not big deal
set(CMAKE_CROSSCOMPILING TRUE)

# gcc
if(NOT DEFINED CMAKE_C_COMPILER)
    find_program(CMAKE_C_COMPILER NAMES ${CROSS_COMPILE_PREFIX}-gcc)
else()
    #message(WARNING "CMAKE_C_COMPILER=${CMAKE_C_COMPILER} is defined")
endif()
# g++
if(NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(CMAKE_CXX_COMPILER NAMES ${CROSS_COMPILE_PREFIX}-g++)
else()
    #message(WARNING "CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} is defined")
endif()
# ld
if(NOT DEFINED CMAKE_LINKER)
    find_program(CMAKE_LINKER NAMES ${CROSS_COMPILE_PREFIX}-ld)
else()
    #message(WARNING "CMAKE_LINKER=${CMAKE_LINKER} is defined")
endif()
# ar
if(NOT DEFINED CMAKE_AR)
    find_program(CMAKE_AR NAMES  ${CROSS_COMPILE_PREFIX}-ar)
else()
    #message(WARNING "CMAKE_AR=${CMAKE_AR} is defined")
endif()

# get sysroot
get_filename_component(_BIN_PATH  ${CMAKE_C_COMPILER} DIRECTORY)
get_filename_component(ARM_SYSROOT "${_BIN_PATH}/../${CROSS_COMPILE_PREFIX}/libc" ABSOLUTE)
set(CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH} ${ARM_SYSROOT})
set(CMAKE_SYSROOT ${ARM_SYSROOT})

set(CMAKE_CXX_FLAGS           "" CACHE INTERAL "")
set(CMAKE_C_FLAGS             "" CACHE INTERAL "")
set(CMAKE_SHARED_LINKER_FLAGS "" CACHE INTERAL "")
set(CMAKE_MODULE_LINKER_FLAGS "" CACHE INTERAL "")
set(CMAKE_EXE_LINKER_FLAGS    "" CACHE INTERAL "")

# explicit pthread arg, or find_package(Threads) may fail
set(THREADS_PTHREAD_ARG "-pthread")

# @option: ARM_ABI, string to select arm abi
if(NOT ARM_ABI)
    # set default
    set(ARM_ABI armeabi-v7a)
endif()

# ABI support
if(ARM_ABI STREQUAL "armeabi-v7a")
    set(CMAKE_SYSTEM_PROCESSOR armv7-a)
    set(_CXX_FLAGS "-march=armv7-a -mfpu=neon")
    set(VALID_FOR_BUILD_ARMV7 true CACHE BOOL "" FORCE)
else()
    message(FATAL_ERROR "UNKNOE ARM_ABI:${ARM_ABI}")
endif()


set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} ${_CXX_FLAGS}")
