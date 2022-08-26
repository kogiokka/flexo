#ifndef LATTICE_EDGE_H
#define LATTICE_EDGE_H

#include <memory>

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
    LatticeEdge(Graphics& gfx, Mesh const& mesh);
    ~LatticeEdge() override;
    void Update(Graphics& gfx) override;
    glm::mat4 GetTransformMatrix() const override;
    std::string GetName() const override;
};

#endif
