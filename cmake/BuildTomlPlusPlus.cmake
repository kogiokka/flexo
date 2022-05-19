if (NOT EXTERNAL_DIR)
    message(FATAL_ERROR "Variable EXTERNAL_DIR is not defined.")
endif()

add_library(tomlplusplus INTERFACE)

target_include_directories(tomlplusplus INTERFACE "${EXTERNAL_DIR}/tomlplusplus/")

