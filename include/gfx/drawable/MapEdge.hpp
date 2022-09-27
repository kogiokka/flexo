#ifndef MAP_EDGE_H
#define MAP_EDGE_H

#include <vector>

#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/bindable/IndexBuffer.hpp"
#include "gfx/drawable/IndexedDrawable.hpp"

class MapEdge : public IndexedDrawable
{
    struct UniformBlock {
        struct Frag {
            STD140_ALIGN glm::vec3 color;
        };

        Frag frag;
    };

    UniformBlock m_ub;

public:
    MapEdge(Graphics& gfx, Mesh const& mesh, std::vector<unsigned int> const& indices);
    ~MapEdge() override;
    void Update(Graphics& gfx) override;
    std::string GetName() const override;
};

#endif
