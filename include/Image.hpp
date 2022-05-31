#pragma once

#include "stb_image.h"

struct Image {
    unsigned char* data;
    int width;
    int height;
    int channels;

    Image();
    Image(char const* filename, int flip = 0, int reqComp = STBI_rgb);
    Image& operator=(Image&& other);
    ~Image();
};
