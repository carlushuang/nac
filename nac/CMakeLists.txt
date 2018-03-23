
add_subdirectory(core)
add_subdirectory(operator)

set(_nac_LOCAL_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/operator
                    ${CMAKE_CURRENT_LIST_DIR}/core PARENT_SCOPE)
set(_nac_EXPORT_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include PARENT_SCOPE)
set(_nac_SRC ${_nac_SRC} PARENT_SCOPE)
set(_nac_EXPORT_HEADER ${CMAKE_CURRENT_LIST_DIR}/include/nac.h PARENT_SCOPE)

if(CMAKE_CROSSCOMPILING AND BUILD_ARMV7 AND (ARM_ABI STREQUAL "armeabi-v7a"))
    add_subdirectory(operator/armv7)
    set(_nac_ARMV7_SRC ${_nac_ARMV7_SRC} PARENT_SCOPE)
endif()