add_library(glad STATIC)
target_sources(glad PRIVATE "src/glad.c")
target_include_directories(glad PUBLIC "include")
add_library(glad::glad ALIAS glad)
