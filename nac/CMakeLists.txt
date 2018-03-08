set(nac_SRC)

add_subdirectory(core)
add_subdirectory(operator)

add_library(nac_obj OBJECT ${nac_SRC})
set_property(TARGET nac_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

target_include_directories(nac_obj PUBLIC include)
target_include_directories(nac_obj PRIVATE operator core)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(nac_obj PRIVATE -std=c++11 -Wall -Wno-switch -Wno-reorder -Wno-sign-compare)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(nac_obj PRIVATE -std=c++11 -Wall -Wno-switch -Wno-reorder -Wno-sign-compare)
endif()

add_library(nac SHARED $<TARGET_OBJECTS:nac_obj>)
add_library(nac-static STATIC $<TARGET_OBJECTS:nac_obj>)

install(TARGETS nac LIBRARY DESTINATION lib )
install(TARGETS nac-static ARCHIVE DESTINATION lib)
install(FILES include/nac.h DESTINATION include)

if(BUILD_TEST)
    add_subdirectory(test)
endif()