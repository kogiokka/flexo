cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project("rvl"
    DESCRIPTION "The Regular VoLumetric format reference library"
    VERSION 0.6.4
    LANGUAGES C)

option(RVL_BUILD_TEST "Build the unit tests for librvl" ON)
option(RVL_BUILD_EXAMPLES "Build the examples of librvl" ON)

add_library(rvl SHARED)
add_library(rvl::rvl ALIAS rvl)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

include("cmake/ExternalProjects.cmake")
include("cmake/PackageSource.cmake")

if (RVL_BUILD_EXAMPLES)
    include("cmake/BuildExamples.cmake")
endif()

set_target_properties(rvl
PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    PUBLIC_HEADER "include/rvl.h"
    VERSION "${rvl_VERSION}"
    SOVERSION "${rvl_VERSION_MAJOR}.${rvl_VERSION_MINOR}"
)

target_compile_definitions(rvl
PRIVATE
    RVL_H_INTERNAL
    $<$<C_COMPILER_ID:MSVC>:RVLLIB_EXPORTS=1>
)
target_compile_options(rvl
PRIVATE
    $<$<C_COMPILER_ID:GNU,CLANG>:-Wall -Wextra -pedantic-errors>
    $<$<C_COMPILER_ID:GNU>:-fvisibility=hidden>
)

target_sources(rvl
PRIVATE
    "src/rvl.c"
    "src/rvl_read.c"
    "src/rvl_write.c"
    "src/rvl_compress.c"
    "src/rvl_get.c"
    "src/rvl_set.c"
    "src/rvl_text.c"
    "src/rvl_log.c"
)

target_include_directories(rvl PRIVATE "include/")

target_include_directories(rvl
INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(rvl
PRIVATE
    liblzma
    lz4
)

set(rvl_DOC_DIR "share/doc/librvl-${rvl_VERSION_MAJOR}.${rvl_VERSION_MINOR}")
set(rvl_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}/librvl")

install(TARGETS rvl EXPORT rvlTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(EXPORT rvlTargets
    DESTINATION "${rvl_INSTALL_LIBDIR}/cmake"
    FILE "librvlTargets.cmake"
    NAMESPACE rvl::
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/librvlConfigVersion.cmake"
    COMPATIBILITY SameMinorVersion # Minor backward compat before the major is 1
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/librvlConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/librvlConfig.cmake"
    INSTALL_DESTINATION "${rvl_INSTALL_LIBDIR}/cmake"
    PATH_VARS rvl_INSTALL_LIBDIR
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/librvlConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/librvlConfigVersion.cmake"
    DESTINATION "${rvl_INSTALL_LIBDIR}/cmake"
)

install(FILES LICENSE NOTICE DESTINATION "${rvl_DOC_DIR}")
install(DIRECTORY licenses DESTINATION "${rvl_DOC_DIR}")

include(CTest)
add_subdirectory("tests/")

