#ifndef TEXTURE_H
#define TEXTURE_H

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"

#include <vector>

namespace Bind
{
    class Texture2D : public Bindable
    {
    protected:
        GLWRPtr<GLWRShaderResourceView> m_resource;
        GLuint m_unit;

    public:
        Texture2D(Graphics& gfx, char const* filename, GLuint unit);
        template <typename T>
        Texture2D(Graphics& gfx, T const* textureData, int width, int height, GLuint unit);
        ~Texture2D() override;
        void Bind() override;

    protected:
        template <typename T>
        void DetermineDataType(GLWRTexture2DDesc& desc) const;
    };

    template <typename T>
    Texture2D::Texture2D(Graphics& gfx, T const* textureData, int width, int height, GLuint unit)
        : Bindable(gfx)
        , m_unit(unit)
    {
        GLWRTexture2DDesc desc;
        GLWRResourceData data;

        desc.width = width;
        desc.height = height;
        desc.textureFormat = GL_RGBA32F;
        desc.pixelFormat = GL_RGBA;

        DetermineDataType<T>(desc);

        data.mem = textureData;

        GLWRPtr<GLWRTexture2D> texture;
        m_gfx->CreateTexture2D(&desc, &data, &texture);

        GLWRShaderResourceViewDesc viewDesc;
        viewDesc.target = GL_TEXTURE_2D;
        viewDesc.format = GL_RGBA32F;
        m_gfx->CreateShaderResourceView(texture.Get(), &viewDesc, &m_resource);
    }

    template <>
    void Texture2D::DetermineDataType<float>(GLWRTexture2DDesc& desc) const;

    template <>
    void Texture2D::DetermineDataType<unsigned char>(GLWRTexture2DDesc& desc) const;
}

#endif
