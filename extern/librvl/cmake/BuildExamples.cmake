add_executable(example)

target_sources(example PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/examples/example.c")
target_include_directories(example PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(example PRIVATE rvl::rvl)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_link_libraries(example PRIVATE m) # math library
endif()
