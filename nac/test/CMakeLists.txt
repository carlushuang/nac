# test
cmake_minimum_required(VERSION 2.8.8)
project(nac_test)

set(test_op_c_fp32_src 
    test_op_c_fp32.cc)

add_executable(nac_test_op_c_fp32 ${test_op_c_fp32_src})
