#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/UniformBlock.hpp"
#include "gfx/drawable/Drawable.hpp"

class LightSource : public Drawable
{
public:
    LightSource(Graphics& gfx, Mesh const& mesh);
    ~LightSource() override;
    void Update(Graphics& gfx) override;

private:
    UniformBlock m_ub;
};

#endif
