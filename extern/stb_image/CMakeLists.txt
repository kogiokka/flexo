add_library(stb_image STATIC)

target_sources(stb_image
PRIVATE
    "stb_image.cpp"
    "stb_image_write.cpp"
)

target_include_directories(stb_image PUBLIC ".")
