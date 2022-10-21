cmake_minimum_required(VERSION 3.22)

project("RVL example")

find_package(librvl 0.4.0 REQUIRED)

add_executable(example)

target_sources(example PRIVATE "./example.c")
target_link_libraries(example PRIVATE rvl::rvl)
