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
