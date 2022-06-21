#ifndef TEXTURE_H
#define TEXTURE_H

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

#include <vector>

class Texture2D : public Bindable
{
protected:
    GLuint id_;

public:
    template <typename T>
    Texture2D(Graphics& gfx, T const* textureData, int width, int height);
    void Bind(Graphics& gfx) override;

protected:
    template <typename T>
    void DetermineDataType(Texture2dDesc& desc) const;
};

template <typename T>
Texture2D::Texture2D(Graphics& gfx, T const* textureData, int width, int height)
{
    Texture2dDesc desc;
    BufferData data;

    desc.width = width;
    desc.height = height;
    desc.textureFormat = GL_RGBA;
    desc.pixelFormat = GL_RGBA;

    DetermineDataType<T>(desc);

    data.mem = textureData;
    gfx.CreateTexture2D(id_, desc, data);
}

template <>
void Texture2D::DetermineDataType<float>(Texture2dDesc& desc) const;

template <>
void Texture2D::DetermineDataType<unsigned char>(Texture2dDesc& desc) const;

#endif
