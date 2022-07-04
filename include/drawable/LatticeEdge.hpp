#ifndef LATTICE_EDGE_H
#define LATTICE_EDGE_H

#include <memory>

#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "Mesh.hpp"
#include "bindable/IndexBuffer.hpp"
#include "drawable/Drawable.hpp"

class LatticeEdge : public Drawable
{
    struct UniformBlock {
        struct Frag {
            STD140_ALIGN glm::vec3 color;
        };

        Frag frag;
    };

    std::shared_ptr<Bind::IndexBuffer> m_ibo;
    UniformBlock m_ub;

public:
    LatticeEdge(Graphics& gfx, Mesh const& mesh);
    void Draw(Graphics& gfx) const override;
    void Update(Graphics& gfx) override;
    glm::mat4 GetTransformMatrix() const override;
};
#endif
