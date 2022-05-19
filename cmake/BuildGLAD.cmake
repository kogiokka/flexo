if (NOT EXTERNAL_DIR)
    message(FATAL_ERROR "Variable EXTERNAL_DIR is not defined.")
endif()

add_library(glad STATIC)

target_sources(glad PRIVATE "${EXTERNAL_DIR}/glad/src/glad.c")
target_include_directories(glad PUBLIC "${EXTERNAL_DIR}/glad/include")
