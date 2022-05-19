#include "Image.hpp"

Image::Image(char const* filename, int flip, int reqComp)
{
    stbi_set_flip_vertically_on_load(flip);
    data = stbi_load(filename, &width, &height, &channels, reqComp);
}

Image::~Image()
{
    stbi_image_free(data);
}
