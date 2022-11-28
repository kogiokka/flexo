cmake_minimum_required(VERSION 3.19)

project("I/O Stream")

add_executable(input_output)
target_sources(input_output PRIVATE "input_output.c")
target_link_libraries(input_output PRIVATE rvl-test)

add_test_cwd("Set I/O Stream" input_output)