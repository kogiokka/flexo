cmake_minimum_required(VERSION 3.12)

project(SOMSurfaceFitting
  VERSION 1.0.3
  DESCRIPTION "A Self-Organizing Map Demo"
  LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Wfloat-conversion")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -D_GLIBCXX_DEBUG")
endif()

find_package(wxWidgets REQUIRED COMPONENTS gl core base)
find_package(glm REQUIRED)

include(${wxWidgets_USE_FILE})
include_directories(include/)
include_directories(ext/glad/include)

add_subdirectory(ext/glad)

file(GLOB sourceFiles CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME}
  ${sourceFiles}
)

if (MSVC)
  target_link_options(${PROJECT_NAME} PUBLIC /SUBSYSTEM:WINDOWS)
endif()

target_link_libraries(${PROJECT_NAME}
  glad
  glm::glm
  ${wxWidgets_LIBRARIES}
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(${PROJECT_NAME} dl)
endif()

include(CPackConfig.cmake)
