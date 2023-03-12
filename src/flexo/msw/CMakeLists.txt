if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_sources(flexo PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/sample.rc")
endif()
