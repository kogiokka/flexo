#include <cassert>
#include <utility>
#include <vector>

#include "Vertex.hpp"
#include "VertexArray.hpp"
#include "World.hpp"
#include "bindable/IndexBuffer.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/ShaderBindable.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"
#include "drawable/LatticeEdge.hpp"

LatticeEdge::LatticeEdge(Graphics& gfx, Mesh const& mesh)
    : ibo_(nullptr)
    , ub_ {}
{
    assert(mesh.HasPositions());

    std::vector<glm::vec3> vertices = mesh.positions;

    ShaderBindable shader(gfx);
    shader.Attach(gfx, ShaderStage::Vert, "shader/LatticeEdge.vert");
    shader.Attach(gfx, ShaderStage::Frag, "shader/LatticeEdge.frag");
    shader.Link(gfx);

    ub_.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    ub_.vert.modelMat = glm::mat4(1.0f);
    ub_.frag.color = glm::vec3(0.7f, 0.7f, 0.7f);

    ibo_ = std::make_shared<IndexBuffer>(gfx, world.latticeEdges);

    AddBind(std::make_shared<Primitive>(gfx, GL_LINES));
    AddBind(std::make_shared<VertexBuffer>(gfx, vertices, VertexAttrib_Position));
    AddBind(ibo_);
    AddBind(std::make_shared<ShaderBindable>(std::move(shader)));
    AddBind(std::make_shared<UniformBuffer<UniformBlock>>(gfx, ub_));
}

void LatticeEdge::Draw(Graphics& gfx) const
{
    Drawable::Draw(gfx);
    gfx.DrawIndexed(ibo_->GetCount());
}

void LatticeEdge::Update(Graphics& gfx)
{
    ub_.vert.viewProjMat = gfx.GetViewProjectionMatrix();

    for (auto it = binds_.begin(); it != binds_.end(); it++) {
        {
            VertexBuffer* vb = dynamic_cast<VertexBuffer*>(it->get());
            if ((vb != nullptr) && (vb->GetStartAttrib() == VertexAttrib_Position)) {
                vb->Update(gfx, world.neurons.positions);
            }
        }
        {
            UniformBuffer<UniformBlock>* ub = dynamic_cast<UniformBuffer<UniformBlock>*>(it->get());
            if (ub != nullptr) {
                ub->Update(gfx, ub_);
            }
        }
    }
}
