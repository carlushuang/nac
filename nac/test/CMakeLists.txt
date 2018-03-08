# test
cmake_minimum_required(VERSION 3.0)
project(nac_test)

find_package(nac REQUIRED)
message(STATUS "nac_INCLUDE_DIRS: ${nac_INCLUDE_DIRS}")
message(STATUS "nac_LIBRARIES: ${nac_LIBRARIES}")

set(test_cxxflags -std=c++11 -Wall -Wno-switch -Wno-reorder -Wno-sign-compare)

# import the lib
add_library(_nac_dynamic SHARED IMPORTED GLOBAL)
#set_target_properties(_nac_dynamic PROPERTIES IMPORTED_LOCATION ${nac_LIBRARIES})
set_property(TARGET _nac_dynamic PROPERTY IMPORTED_LOCATION "${nac_LIBRARIES}")

# test_op_c_fp32_src
set(test_op_c_fp32_src 
    test_op_c_fp32.cc)

add_executable(nac_test_op_c_fp32 ${test_op_c_fp32_src})
target_compile_options(nac_test_op_c_fp32 PRIVATE ${test_cxxflags})
target_include_directories(nac_test_op_c_fp32 PRIVATE ${nac_INCLUDE_DIRS})
target_link_libraries(nac_test_op_c_fp32 _nac_dynamic)

