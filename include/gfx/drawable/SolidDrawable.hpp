#ifndef SOLID_DRAWABLE_H
#define SOLID_DRAWABLE_H

#include <unordered_map>

#include "gfx/Graphics.hpp"
#include "gfx/UniformBlock.hpp"
#include "gfx/drawable/Drawable.hpp"

class SolidDrawable : public Drawable
{
public:
    SolidDrawable(Graphics& gfx, Mesh const& mesh);
    ~SolidDrawable() override;
    void Update(Graphics& gfx) override;

private:
    std::unordered_map<std::string, UniformBlock> m_ubs;
};

#endif
