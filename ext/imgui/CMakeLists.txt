include_directories(.)

file(GLOB SOURCE CONFIGURE_DEPENDS
    *.h
    *.cpp
)

add_library(imgui STATIC
    ${SOURCE}
)
