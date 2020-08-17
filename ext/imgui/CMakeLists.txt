cmake_minimum_required(VERSION 3.12)

file(GLOB sourceFiles CONFIGURE_DEPENDS
    *.h
    *.cpp
)

add_library(imgui STATIC
    ${sourceFiles}
)

