include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET FALSE)

add_subdirectory("librvl")
add_subdirectory("glad")
add_subdirectory("stb_image")
add_subdirectory("glm")
add_subdirectory("wxWidgets")
