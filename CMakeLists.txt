cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project("SOM Demo"
  VERSION 1.0.3
  DESCRIPTION "A Self-Organizing Map Demo"
  LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(wxWidgets 3.1 COMPONENTS gl core REQUIRED)
find_package(glm REQUIRED)
include(${wxWidgets_USE_FILE})

add_executable(som-demo)
add_library(glad STATIC)

if (CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(som-demo PRIVATE -Wall -Wextra --pedantic-errors)
    target_compile_definitions(som-demo PRIVATE _GLIBCXX_DEBUG)
endif()

file(GLOB sourceFiles CONFIGURE_DEPENDS "src/*.cpp")

target_sources(som-demo PRIVATE ${sourceFiles})
target_sources(glad PRIVATE "ext/glad/src/glad.c")
target_include_directories(som-demo PRIVATE include/)
target_include_directories(glad PUBLIC ext/glad/include)

if (MSVC)
    target_link_options(som-demo PUBLIC /SUBSYSTEM:WINDOWS)
endif()

target_link_libraries(som-demo PRIVATE
    glad
    glm::glm
    ${wxWidgets_LIBRARIES}
    ${CMAKE_DL_LIBS}
)

include(CPackConfig.cmake)
