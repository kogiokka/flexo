cmake_minimum_required(VERSION 3.22)

project("RVL example")

find_package(librvl 0.6 REQUIRED)

add_executable(example)

target_sources(example PRIVATE "./example.c")
target_link_libraries(example PRIVATE rvl::rvl)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_link_libraries(example PRIVATE m) # math library
endif()

# Copy librvl dynamic library to the executable's directory
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_custom_command (TARGET example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:rvl::rvl> $<TARGET_FILE_DIR:example>
    )
endif()
