#ifndef LATTICE_EDGE_H
#define LATTICE_EDGE_H

#include <vector>

#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "Mesh.hpp"
#include "bindable/IndexBuffer.hpp"
#include "drawable/IndexedDrawable.hpp"

class LatticeEdge : public IndexedDrawable
{
    struct UniformBlock {
        struct Frag {
            STD140_ALIGN glm::vec3 color;
        };

        Frag frag;
    };

    UniformBlock m_ub;

public:
    LatticeEdge(Graphics& gfx, Mesh const& mesh, std::vector<unsigned int> const& indices);
    ~LatticeEdge() override;
    void Update(Graphics& gfx) override;
    std::string GetName() const override;
};

#endif
