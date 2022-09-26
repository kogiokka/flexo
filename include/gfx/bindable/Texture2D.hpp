#ifndef TEXTURE_H
#define TEXTURE_H

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"

#include <string>
#include <vector>

namespace Bind
{
    class Texture2D : public Bindable
    {
    protected:
        GLWRPtr<IGLWRShaderResourceView> m_resource;
        GLuint m_unit;

    public:
        Texture2D(Graphics& gfx, char const* filename, GLuint unit);
        template <typename T>
        Texture2D(Graphics& gfx, T const* textureData, int width, int height, GLuint unit);
        ~Texture2D() override;
        void Bind(Graphics& gfx) override;
        static std::string GenerateUID(char const* filename, GLuint unit);

    protected:
        template <typename T>
        void DetermineDataType(GLWRTexture2DDesc& desc) const;
    };

    template <typename T>
    Texture2D::Texture2D(Graphics& gfx, T const* textureData, int width, int height, GLuint unit)
        : m_unit(unit)
    {
        GLWRTexture2DDesc desc;
        GLWRResourceData data;

        desc.width = width;
        desc.height = height;
        desc.internalFormat = GL_RGBA32F;
        desc.pixelFormat = GL_RGBA;

        DetermineDataType<T>(desc);

        data.mem = textureData;

        GLWRPtr<IGLWRTexture2D> texture;
        gfx.CreateTexture2D(&desc, &data, &texture);

        GLWRShaderResourceViewDesc viewDesc;
        viewDesc.type = GLWRShaderResourceViewType_Texture2D;
        viewDesc.format = GL_RGBA32F;
        gfx.CreateShaderResourceView(texture.Get(), &viewDesc, &m_resource);
        gfx.GenerateMips(m_resource.Get());
    }

    template <>
    void Texture2D::DetermineDataType<float>(GLWRTexture2DDesc& desc) const;

    template <>
    void Texture2D::DetermineDataType<unsigned char>(GLWRTexture2DDesc& desc) const;
}

#endif
