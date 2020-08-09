cmake_minimum_required(VERSION 3.12)

project(SOMSurfaceFitting LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -D_GLIBCXX_DEBUG")
endif()

include_directories(include/)
include_directories(ext/SDL2/include)
include_directories(ext/glad/include)
include_directories(ext/glm/include)
include_directories(ext/imgui)

add_subdirectory(ext/glad)
add_subdirectory(ext/glm)
add_subdirectory(ext/imgui)

add_library(SDL2-static INTERFACE IMPORTED)
add_library(SDL2main INTERFACE IMPORTED)
set_property(TARGET SDL2-static PROPERTY INTERFACE_LINK_LIBRARIES "${PROJECT_SOURCE_DIR}/lib/libSDL2.a")
set_property(TARGET SDL2main PROPERTY INTERFACE_LINK_LIBRARIES "${PROJECT_SOURCE_DIR}/lib/libSDL2main.a")

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
    pthread
    m
    SDL2-static
    SDL2main
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} dl)
endif()
