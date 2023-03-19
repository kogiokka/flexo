add_library(log)
target_sources(log PRIVATE "Logger.c")
target_include_directories(log PUBLIC include)
add_library(flexo::log ALIAS log)
