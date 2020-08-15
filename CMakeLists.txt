cmake_minimum_required(VERSION 3.12)

project(SOMSurfaceFitting LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -D_GLIBCXX_DEBUG")
endif()

find_package(SDL2 REQUIRED)
if (SDL2_FOUND)
    message(STATUS "SDL2 Include Path: ${SDL2_INCLUDE_DIRS}")
    message(STATUS "SDL2 Libraries: ${SDL2_LIBRARIES}")
endif()

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(include/)
include_directories(ext/glad/include)
include_directories(ext/glm/include)
include_directories(ext/imgui)

add_subdirectory(ext/glad)
add_subdirectory(ext/glm)
add_subdirectory(ext/imgui)

file(GLOB SOURCE CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(${PROJECT_NAME}
    ${SOURCE}
)

target_link_libraries(${PROJECT_NAME}
    glad
    glm
    imgui
    SDL2::SDL2-static
    SDL2::SDL2main
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} dl)
endif()
