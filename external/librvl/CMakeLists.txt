cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project("rvl"
    DESCRIPTION "The Regular VoLumetric format reference library"
    VERSION 0.2.1
    LANGUAGES C)

option(RVL_BUILD_TEST "Build the unit tests for librvl" ON)

add_library(rvl SHARED)
add_library(rvl::rvl ALIAS rvl)

include(GNUInstallDirs)
include(FetchContent)

include("cmake/FetchLZ4.cmake")
include("cmake/PackageSource.cmake")
add_subdirectory("external/")

set_target_properties(rvl PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

target_compile_options(rvl PRIVATE
    $<$<C_COMPILER_ID:GNU,CLANG>:-Wall -Wextra -pedantic-errors>
)

target_sources(rvl PRIVATE
    "src/rvl_write.c"
    "src/rvl_read.c"
    "src/rvl_common.c"
    "src/rvl_text.c"
    "src/rvl_get.c"
    "src/rvl_set.c"

    "src/detail/rvl_write_p.c"
    "src/detail/rvl_read_p.c"
)

target_include_directories(rvl PRIVATE "include/")

add_custom_target(rvl-copy-public-header
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/include/rvl.h"
            "${CMAKE_CURRENT_BINARY_DIR}/include/rvl.h"
)
add_dependencies(rvl rvl-copy-public-header)

set(RVL_PUBLIC_HEADERS_EXPORT_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")
target_include_directories(rvl INTERFACE "${RVL_PUBLIC_HEADERS_EXPORT_DIR}")
target_link_libraries(rvl PRIVATE lz4_static log.c)

install(TARGETS rvl
    EXPORT rvlTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

export(EXPORT rvlTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/rvlTargets.cmake"
    NAMESPACE rvl::
)

if (RVL_BUILD_TEST)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()
