#ifndef TEXTURED_DRAWABLE_H
#define TEXTURED_DRAWABLE_H

#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/UniformBlock.hpp"
#include "gfx/drawable/Drawable.hpp"

class TexturedDrawable : public Drawable
{
public:
    TexturedDrawable(Graphics& gfx, Mesh const& mesh);
    ~TexturedDrawable() override;
    void ChangeTexture(Graphics& gfx, char const* filename);
    void Update(Graphics& gfx) override;

private:
    UniformBlock m_ublight;
    UniformBlock m_ubmat;
    UniformBlock m_ubo;
};

#endif
