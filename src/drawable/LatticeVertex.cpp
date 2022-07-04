#include <utility>
#include <vector>

#include "Vertex.hpp"
#include "World.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"
#include "bindable/VertexLayout.hpp"
#include "bindable/program/FragmentShaderProgram.hpp"
#include "bindable/program/ProgramPipeline.hpp"
#include "bindable/program/VertexShaderProgram.hpp"
#include "drawable/LatticeVertex.hpp"

LatticeVertex::LatticeVertex(Graphics& gfx, Mesh const& mesh)
    : m_count(0)
    , m_ub {}
{
    std::vector<AttributeDesc> attrs = {
        { "position", 3, GL_FLOAT, GL_FALSE }, // 0
        { "normal", 3, GL_FLOAT, GL_FALSE }, // 1
        { "translation", 3, GL_FLOAT, GL_FALSE }, // 2
    };

    std::vector<VertexPN> vertices;
    for (unsigned int i = 0; i < mesh.positions.size(); i++) {
        VertexPN v;
        v.position = mesh.positions[i];
        v.normal = mesh.normals[i];
        vertices.push_back(v);
    }

    m_count = vertices.size();

    m_ub.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    m_ub.vert.modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.2f);
    m_ub.frag.alpha = world.modelColorAlpha;
    m_ub.frag.viewPos = gfx.GetCameraPosition();
    m_ub.frag.light.position = gfx.GetCameraPosition();
    m_ub.frag.light.ambient = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.frag.light.diffusion = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.frag.light.specular = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.frag.material.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.frag.material.diffusion = glm::vec3(0.6f, 0.6f, 0.6f);
    m_ub.frag.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.frag.material.shininess = 256.0f;

    auto pipeline = std::make_shared<Bind::ProgramPipeline>(gfx);
    AddBind(std::make_shared<Bind::VertexLayout>(gfx, attrs));
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices, 0));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, world.neurons.positions, 2));
    AddBind(pipeline);
    AddBind(std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/LatticeVertex.vert", pipeline->GetId()));
    AddBind(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/LatticeVertex.frag", pipeline->GetId()));
    AddBind(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub));
}

// FIXME: VertexLayout configurations
void LatticeVertex::Draw(Graphics& gfx) const
{
    if (!m_isVisible) {
        return;
    }
    Drawable::Draw(gfx);
    glVertexBindingDivisor(2, 1);
    gfx.DrawInstanced(m_count, world.neurons.positions.size());
}

void LatticeVertex::Update(Graphics& gfx)
{
    m_isVisible = world.renderFlags & RenderFlag_DrawLatticeVertex;

    m_ub.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    m_ub.frag.alpha = world.modelColorAlpha;
    m_ub.frag.viewPos = gfx.GetCameraPosition();
    m_ub.frag.light.position = gfx.GetCameraPosition();

    for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
        {
            Bind::VertexBuffer* vb = dynamic_cast<Bind::VertexBuffer*>(it->get());
            if ((vb != nullptr) && (vb->GetStartAttrib() == 2)) {
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
