#ifndef TEXTURED_DRAWABLE_H
#define TEXTURED_DRAWABLE_H

#include <unordered_map>

#include "Mesh.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/UniformBlock.hpp"
#include "gfx/bindable/Texture2D.hpp"
#include "gfx/drawable/Drawable.hpp"

class TexturedDrawable : public Drawable
{
public:
    TexturedDrawable(Graphics& gfx, Mesh const& mesh, std::shared_ptr<Bind::Texture2D> texture);
    ~TexturedDrawable() override;
    void ChangeTexture(std::shared_ptr<Bind::Texture2D> texture);
};

#endif
