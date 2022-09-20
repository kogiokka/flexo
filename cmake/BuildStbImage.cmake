if (NOT EXTERNAL_DIR)
    message(FATAL_ERROR "Variable EXTERNAL_DIR is not defined.")
endif()

add_library(stb_image STATIC)

target_sources(stb_image
PRIVATE
    "${EXTERNAL_DIR}/stb_image/stb_image.cpp"
    "${EXTERNAL_DIR}/stb_image/stb_image_write.cpp"
)

target_include_directories(stb_image PUBLIC "${EXTERNAL_DIR}/stb_image")
