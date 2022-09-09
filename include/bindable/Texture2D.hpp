#ifndef TEXTURE_H
#define TEXTURE_H

#include "bindable/Bindable.hpp"
#include "gfx/Graphics.hpp"

#include <vector>

namespace Bind
{
    class Texture2D : public Bindable
    {
    protected:
        GLuint m_id;
        GLuint m_unit;

    public:
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
        , m_id(0)
        , m_unit(unit)
    {
        GLWRTexture2DDesc desc;
        GLWRResourceData data;

        desc.width = width;
        desc.height = height;
        desc.textureFormat = GL_RGBA;
        desc.pixelFormat = GL_RGBA;

        DetermineDataType<T>(desc);

        data.mem = textureData;
        m_gfx->CreateTexture2D(m_id, m_unit, desc, data);
    }

    template <>
    void Texture2D::DetermineDataType<float>(GLWRTexture2DDesc& desc) const;

    template <>
    void Texture2D::DetermineDataType<unsigned char>(GLWRTexture2DDesc& desc) const;
}

#endif
