#include <cassert>
#include <utility>
#include <vector>

#include "Vertex.hpp"
#include "World.hpp"
#include "bindable/IndexBuffer.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/TransformUniformBuffer.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"
#include "bindable/VertexLayout.hpp"
#include "bindable/program/FragmentShaderProgram.hpp"
#include "bindable/program/ProgramPipeline.hpp"
#include "bindable/program/VertexShaderProgram.hpp"
#include "drawable/LatticeEdge.hpp"

LatticeEdge::LatticeEdge(Graphics& gfx, Mesh const& mesh)
    : m_ibo(nullptr)
    , m_ub {}
{
    std::vector<AttributeDesc> attrs = {
        { "position", 3, GL_FLOAT, GL_FALSE },
    };

    std::vector<glm::vec3> vertices = mesh.positions;

    m_ub.frag.color = glm::vec3(0.7f, 0.7f, 0.7f);

    m_ibo = std::make_shared<Bind::IndexBuffer>(gfx, world.latticeEdges);

    auto pipeline = std::make_shared<Bind::ProgramPipeline>(gfx);
    AddBind(std::make_shared<Bind::VertexLayout>(gfx, attrs));
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_LINES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));
    AddBind(m_ibo);
    AddBind(pipeline);
    AddBind(std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/LatticeEdge.vert", pipeline->GetId()));
    AddBind(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/LatticeEdge.frag", pipeline->GetId()));
    AddBind(std::make_shared<Bind::TransformUniformBuffer>(gfx, *this));
    AddBind(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub, 1));
}

// FIXME: VertexLayout configurations
void LatticeEdge::Draw(Graphics& gfx) const
{
    if (!m_isVisible) {
        return;
    }
    Drawable::Draw(gfx);
    gfx.DrawIndexed(m_ibo->GetCount());
}

void LatticeEdge::Update(Graphics&)
{
    m_isVisible = world.renderFlags & RenderFlag_DrawLatticeEdge;

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

glm::mat4 LatticeEdge::GetTransformMatrix() const
{
    return glm::mat4(1.0f);
}
