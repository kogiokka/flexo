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
    public:
        Texture2D(Graphics& gfx, std::string const& filename, GLuint unit);
        ~Texture2D() override;
        void Bind(Graphics& gfx) override;
        static std::string GenerateUID(std::string const& filename, GLuint unit);

    protected:
        GLWRPtr<IGLWRShaderResourceView> m_resource;
        GLuint m_unit;
    };
}

#endif
