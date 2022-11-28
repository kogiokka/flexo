cmake_minimum_required(VERSION 3.19)

project("Tiny Data")

add_executable(tiny_data)
target_sources(tiny_data PRIVATE "tiny_data.c")
target_link_libraries(tiny_data PRIVATE rvl-test)

add_test_cwd("Read/Write tiny data" tiny_data)
