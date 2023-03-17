add_library(stb_image STATIC)

target_sources(stb_image
PRIVATE
    "image.cpp"
    "image_write.cpp"
)

target_include_directories(stb_image PUBLIC "include")
add_library(stb::image ALIAS stb_image)
