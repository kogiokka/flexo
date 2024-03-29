#include "gfx/bindable/Texture2D.hpp"

namespace Bind
{
    Texture2D::Texture2D(Graphics& gfx, std::string const& filename, GLuint unit)
        : m_unit(unit)
    {
        Graphics::CreateShaderResourceViewFromFile(&gfx, filename.c_str(), &m_resource);
        m_name = GenerateUID(filename, unit);
    }

    Texture2D::~Texture2D()
    {
    }

    void Texture2D::Bind(Graphics& gfx)
    {
        gfx.SetShaderResources(m_unit, 1, m_resource.GetAddressOf());
    }

    std::string const& Texture2D::GetName() const
    {
        return m_name;
    }

    std::string Texture2D::GenerateUID(std::string const& filename, GLuint unit)
    {
        return filename + "#" + std::to_string(unit);
    }
}
