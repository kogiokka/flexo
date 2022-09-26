#include "gfx/bindable/Texture2D.hpp"

#include "common/Logger.hpp"

namespace Bind
{
    Texture2D::Texture2D(Graphics& gfx, char const* filename, GLuint unit)
        : m_unit(unit)
    {
        Graphics::CreateShaderResourceViewFromFile(&gfx, filename, &m_resource);
    }

    Texture2D::~Texture2D()
    {
    }

    void Texture2D::Bind(Graphics& gfx)
    {
        gfx.SetShaderResources(m_unit, 1, m_resource.GetAddressOf());
    }

    std::string Texture2D::GenerateUID(char const* filename, GLuint unit)
    {
        return std::string(filename) + std::to_string(unit);
    }

    template <>
    void Texture2D::DetermineDataType<float>(GLWRTexture2DDesc& desc) const
    {
        desc.dataType = GL_FLOAT;
    }

    template <>
    void Texture2D::DetermineDataType<unsigned char>(GLWRTexture2DDesc& desc) const
    {
        desc.dataType = GL_UNSIGNED_BYTE;
    }
}
