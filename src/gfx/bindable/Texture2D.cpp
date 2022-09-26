#include "gfx/bindable/Texture2D.hpp"

#include "common/Logger.hpp"

namespace Bind
{
    Texture2D::Texture2D(Graphics& gfx, std::string const& filename, GLuint unit)
        : m_unit(unit)
    {
        Graphics::CreateShaderResourceViewFromFile(&gfx, filename.c_str(), &m_resource);
    }

    Texture2D::~Texture2D()
    {
    }

    void Texture2D::Bind(Graphics& gfx)
    {
        gfx.SetShaderResources(m_unit, 1, m_resource.GetAddressOf());
    }

    std::string Texture2D::GenerateUID(std::string const& filename, GLuint unit)
    {
        return filename + "#" + std::to_string(unit);
    }
}
