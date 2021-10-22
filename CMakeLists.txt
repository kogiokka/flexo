cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project("SOM Demo"
  VERSION 1.0.3
  DESCRIPTION "A Self-Organizing Map Demo"
  LANGUAGES CXX C)

find_package(wxWidgets 3.1 COMPONENTS gl core REQUIRED)
find_package(glm REQUIRED)
include(${wxWidgets_USE_FILE})

add_executable(som-demo)
add_library(glad STATIC)

target_compile_features(som-demo PRIVATE cxx_std_17)
set_target_properties(som-demo PROPERTIES CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(som-demo PRIVATE -Wall -Wextra --pedantic-errors)
    target_compile_definitions(som-demo PRIVATE _GLIBCXX_DEBUG)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(som-demo PRIVATE -Wall -Wextra --pedantic-errors)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_options(som-demo PUBLIC /SUBSYSTEM:WINDOWS)
endif()

file(GLOB sourceFiles CONFIGURE_DEPENDS "src/*.cpp")

target_sources(som-demo PRIVATE ${sourceFiles})
target_sources(glad PRIVATE "ext/glad/src/glad.c")
target_include_directories(som-demo PRIVATE include/)
target_include_directories(glad PUBLIC ext/glad/include)

target_link_libraries(som-demo PRIVATE
    glad
    glm::glm
    ${wxWidgets_LIBRARIES}
    ${CMAKE_DL_LIBS}
)

add_custom_target(symlink-res
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${CMAKE_SOURCE_DIR}/res"
        "${CMAKE_BINARY_DIR}/res"
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${CMAKE_SOURCE_DIR}/shader"
        "${CMAKE_BINARY_DIR}/shader"
    DEPENDS
        "${CMAKE_SOURCE_DIR}/res"
    COMMENT "Creating symlink from build tree to project resources..."
    VERBATIM
)

add_dependencies(som-demo symlink-res)

include(CPackConfig.cmake)
