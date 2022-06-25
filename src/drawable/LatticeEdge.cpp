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
    : m_ibo(nullptr)
    , m_ub {}
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

    m_ub.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    m_ub.vert.modelMat = glm::mat4(1.0f);
    m_ub.frag.color = glm::vec3(0.7f, 0.7f, 0.7f);

    m_ibo = std::make_shared<Bind::IndexBuffer>(gfx, world.latticeEdges);

    AddBind(std::make_shared<Bind::VertexLayout>(gfx, attrs));
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_LINES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));
    AddBind(m_ibo);
    AddBind(shader);
    AddBind(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub));
}

void LatticeEdge::Draw(Graphics& gfx) const
{
    Drawable::Draw(gfx);
    gfx.DrawIndexed(m_ibo->GetCount());
}

void LatticeEdge::Update(Graphics& gfx)
{
    m_ub.vert.viewProjMat = gfx.GetViewProjectionMatrix();

    for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
        {
            Bind::VertexBuffer* vb = dynamic_cast<Bind::VertexBuffer*>(it->get());
            if ((vb != nullptr) && (vb->GetStartAttrib() == 0)) {
                vb->Update(world.neurons.positions);
            }
        }
        {
            Bind::UniformBuffer<UniformBlock>* ub = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
            if (ub != nullptr) {
                ub->Update(m_ub);
            }
        }
    }
}
