#include <cassert>
#include <utility>
#include <vector>

#include "Vertex.hpp"
#include "World.hpp"
#include "bindable/IndexBuffer.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/Shader.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"
#include "bindable/VertexLayout.hpp"
#include "drawable/LatticeEdge.hpp"

LatticeEdge::LatticeEdge(Graphics& gfx, Mesh const& mesh)
    : ibo_(nullptr)
    , ub_ {}
{
    assert(mesh.HasPositions());

    std::vector<AttributeDesc> attrs = {
        { "position", 3, GL_FLOAT, GL_FALSE },
    };

    std::vector<glm::vec3> vertices = mesh.positions;

    auto shader = std::make_shared<Bind::Shader>(gfx);
    shader->Attach(ShaderStage::Vert, "shader/LatticeEdge.vert");
    shader->Attach(ShaderStage::Frag, "shader/LatticeEdge.frag");
    shader->Link();

    ub_.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    ub_.vert.modelMat = glm::mat4(1.0f);
    ub_.frag.color = glm::vec3(0.7f, 0.7f, 0.7f);

    ibo_ = std::make_shared<Bind::IndexBuffer>(gfx, world.latticeEdges);

    AddBind(std::make_shared<Bind::VertexLayout>(gfx, attrs));
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_LINES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));
    AddBind(ibo_);
    AddBind(shader);
    AddBind(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, ub_));
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
            Bind::VertexBuffer* vb = dynamic_cast<Bind::VertexBuffer*>(it->get());
            if ((vb != nullptr) && (vb->GetStartAttrib() == 0)) {
                vb->Update(world.neurons.positions);
            }
        }
        {
            Bind::UniformBuffer<UniformBlock>* ub = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
            if (ub != nullptr) {
                ub->Update(ub_);
            }
        }
    }
}
