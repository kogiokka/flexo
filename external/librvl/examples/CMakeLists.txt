cmake_minimum_required(VERSION 3.22)

project("RVL example")

find_package(librvl 0.6.0 REQUIRED)

add_executable(example)

target_sources(example PRIVATE "./example.c")
target_link_libraries(example PRIVATE rvl::rvl)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_link_libraries(example PRIVATE m) # math library
endif()
