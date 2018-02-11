# test
cmake_minimum_required(VERSION 2.8)
project(test_op_c_fp32)

set(test_op_c_fp32_src 
    test_op_c_fp32.cc)

add_executable(test_op_c_fp32 ${test_op_c_fp32_src})
