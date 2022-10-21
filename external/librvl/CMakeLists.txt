cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project("rvl"
    DESCRIPTION "The Regular VoLumetric format reference library"
    VERSION 0.4.0
    LANGUAGES C)

option(RVL_BUILD_TEST "Build the unit tests for librvl" ON)

add_library(rvl SHARED)
add_library(rvl::rvl ALIAS rvl)

include(GNUInstallDirs)
include(FetchContent)
include(CMakePackageConfigHelpers)

include("cmake/FetchLZ4.cmake")
include("cmake/PackageSource.cmake")
add_subdirectory("external/")

set_target_properties(rvl PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    PUBLIC_HEADER "include/rvl.h"
)

target_compile_options(rvl PRIVATE
    $<$<C_COMPILER_ID:GNU,CLANG>:-Wall -Wextra -pedantic-errors>
)

target_sources(rvl PRIVATE
    "src/rvl.c"
    "src/rvl_read.c"
    "src/rvl_write.c"
    "src/rvl_get.c"
    "src/rvl_set.c"
    "src/rvl_text.c"
)

add_custom_target(rvl-public-header-export
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/include/rvl.h"
            "${CMAKE_CURRENT_BINARY_DIR}/include/rvl.h"
)

add_dependencies(rvl rvl-public-header-export)

target_include_directories(rvl
PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
PUBLIC
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>" # For exporting without installation
)

target_link_libraries(rvl PRIVATE
    lz4_static
    log.c
)

install(TARGETS rvl EXPORT librvlTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
install(EXPORT librvlTargets
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/librvl/cmake"
    NAMESPACE rvl::
)
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/librvl")
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/librvlConfigVersion.cmake"
  COMPATIBILITY SameMinorVersion # Minor backward compat before the major is 1
)
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/librvlConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/librvlConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/librvl/cmake"
  PATH_VARS LIB_INSTALL_DIR
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/librvlConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/librvlConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/librvl/cmake"
)

export(EXPORT librvlTargets
    NAMESPACE rvl::
)

if (RVL_BUILD_TEST)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()
