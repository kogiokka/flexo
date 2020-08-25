cmake_minimum_required(VERSION 3.12)

project(SOMSurfaceFitting LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -D_GLIBCXX_DEBUG")
endif()

find_package(wxWidgets REQUIRED COMPONENTS gl core)

include(${wxWidgets_USE_FILE})
include_directories(include/)
include_directories(ext/glad/include)
include_directories(ext/glm/include)

add_subdirectory(ext/glad)
add_subdirectory(ext/glm)

file(GLOB SOURCE CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(${PROJECT_NAME}
    ${SOURCE}
)

target_link_libraries(${PROJECT_NAME}
    glad
    glm
    ${wxWidgets_LIBRARIES}
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} dl)
endif()
