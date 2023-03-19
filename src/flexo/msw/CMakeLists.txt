add_library(msw INTERFACE)
add_library(flexo::msw ALIAS msw)
target_sources(msw INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/sample.rc")
