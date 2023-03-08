cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project("Flexo"
    VERSION 0.1.0
    DESCRIPTION "Texture Mapping for Voxel-based Models"
    LANGUAGES C CXX
)

find_package(OpenGL REQUIRED)
find_package(OpenMP QUIET)
find_package(glm QUIET)
find_package(wxWidgets CONFIG COMPONENTS base core gl aui QUIET)

add_subdirectory("extern")
add_subdirectory("src")

include(CPackConfig.cmake)
