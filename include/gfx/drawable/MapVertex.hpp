#ifndef MAP_VERTEX_H
#define MAP_VERTEX_H

#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/UniformBlock.hpp"
#include "gfx/bindable/Texture2D.hpp"
#include "gfx/drawable/InstancedDrawable.hpp"

class MapVertex : public InstancedDrawable
{
public:
    MapVertex(Graphics& gfx, Mesh const& instanceMesh, std::vector<glm::vec3> const& perInstanceData);
    ~MapVertex() override;
    void Update(Graphics& gfx) override;

private:
    UniformBlock m_ublight;
    UniformBlock m_ubmat;
    UniformBlock m_ubo;
};
#endif
