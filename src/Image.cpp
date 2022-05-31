#include <cstring>

#include "Image.hpp"

Image::Image()
    : data(nullptr)
    , width(0)
    , height(0)
    , channels(0)
{
}

Image& Image::operator=(Image&& other)
{
    if (this == &other) {
        return *this;
    }

    stbi_image_free(data);

    data = other.data;
    width = other.width;
    height = other.height;
    channels = other.channels;

    other.data = nullptr;
    other.width = 0;
    other.height = 0;
    other.channels = 0;

    return *this;
}

Image::Image(char const* filename, int flip, int reqComp)
{
    stbi_set_flip_vertically_on_load(flip);
    data = stbi_load(filename, &width, &height, &channels, reqComp);
}

Image::~Image()
{
    stbi_image_free(data);
}
