cmake_minimum_required(VERSION 3.22)

project("log.c")
add_library(log.c STATIC)

target_sources(log.c PRIVATE "log.c/src/log.c")
target_include_directories(log.c PUBLIC "log.c/src/")
target_compile_definitions(log.c PRIVATE LOG_USE_COLOR)
set_target_properties(log.c PROPERTIES POSITION_INDEPENDENT_CODE ON)
