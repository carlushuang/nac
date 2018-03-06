set(nac_SRC)

add_subdirectory(core)
add_subdirectory(operator)

add_library(nac ${nac_SRC})
target_include_directories(nac PUBLIC include)
target_include_directories(nac PRIVATE operator)
target_compile_options(nac PRIVATE "-std=c++11")
