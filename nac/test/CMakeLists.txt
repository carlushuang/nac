# test
cmake_minimum_required(VERSION 3.0)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # we launch this list file standalone, so we need find nac exported targets
    project(nac_test)
    find_package(nac REQUIRED)
endif()

#set(test_opt -Os -ffast-math)
set(test_opt -Og)
set(test_cxxflags -std=c++11 -Wall -Wno-switch -Wno-reorder -Wno-sign-compare)

# test_op_c_fp32_src
set(test_op_c_fp32_src 
    test_op_c_fp32.cc)

add_executable(nac_test_op_c_fp32 ${test_op_c_fp32_src})
target_compile_options(nac_test_op_c_fp32 PRIVATE ${test_cxxflags})
target_link_libraries(nac_test_op_c_fp32 PRIVATE nac::nac)


# test_op_armv7_fp32
if(CMAKE_CROSSCOMPILING AND BUILD_ARMV7 AND (ARM_ABI STREQUAL "armeabi-v7a"))
    set(test_op_armv7_fp32_src test_op_armv7_fp32.cc)
    add_executable(test_op_armv7_fp32 ${test_op_armv7_fp32_src})
    target_link_libraries(test_op_armv7_fp32 PRIVATE nac::nac)
endif()

# test brew
set(test_brew_src test_brew.cc)
add_executable(test_brew ${test_brew_src})
target_compile_options(test_brew PRIVATE ${test_cxxflags})
target_link_libraries(test_brew PRIVATE nac::nac)