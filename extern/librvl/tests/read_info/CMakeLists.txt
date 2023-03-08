cmake_minimum_required(VERSION 3.19)

project("Read Info")

add_executable(read_info)
target_sources(read_info PRIVATE "read_info.c")
target_link_libraries(read_info PRIVATE rvl-test)

add_test_cwd("Read information from file" read_info)
