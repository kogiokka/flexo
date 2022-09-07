#ifndef TEXTURE_H
#define TEXTURE_H

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

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
        void DetermineDataType(Texture2dDesc& desc) const;
    };

    template <typename T>
    Texture2D::Texture2D(Graphics& gfx, T const* textureData, int width, int height, GLuint unit)
        : Bindable(gfx)
        , m_id(0)
        , m_unit(unit)
    {
        Texture2dDesc desc;
        ResourceData data;

        desc.width = width;
        desc.height = height;
        desc.textureFormat = GL_RGBA;
        desc.pixelFormat = GL_RGBA;

        DetermineDataType<T>(desc);

        data.mem = textureData;
        m_gfx->CreateTexture2D(m_id, m_unit, desc, data);
    }

    template <>
    void Texture2D::DetermineDataType<float>(Texture2dDesc& desc) const;

    template <>
    void Texture2D::DetermineDataType<unsigned char>(Texture2dDesc& desc) const;
}

#endif
