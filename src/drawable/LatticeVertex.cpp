#include <utility>
#include <vector>

#include "Vertex.hpp"
#include "World.hpp"
#include "bindable/InputLayout.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/RasterizerState.hpp"
#include "bindable/TransformUniformBuffer.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"
#include "bindable/program/FragmentShaderProgram.hpp"
#include "bindable/program/ProgramPipeline.hpp"
#include "bindable/program/VertexShaderProgram.hpp"
#include "drawable/LatticeVertex.hpp"

LatticeVertex::LatticeVertex(Graphics& gfx, Mesh const& instanceMesh, Mesh const& perInstanceData)
    : m_ub {}
{
    std::vector<InputElementDesc> inputs = {
        { "position", InputFormat::Float3, 0, offsetof(VertexPN, position), InputClassification::PerVertex, 0 },
        { "normal", InputFormat::Float3, 0, offsetof(VertexPN, normal), InputClassification::PerVertex, 0 },
        { "translation", InputFormat::Float3, 2, 0, InputClassification::PerInstance, 1 },
    };

    std::vector<VertexPN> vertices;
    for (unsigned int i = 0; i < instanceMesh.positions.size(); i++) {
        VertexPN v;
        v.position = instanceMesh.positions[i];
        v.normal = instanceMesh.normals[i];
        vertices.push_back(v);
    }

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
    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/LatticeVertex.vert", *pipeline);
    auto fs = std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/LatticeVertex.frag", *pipeline);
    AddBind(pipeline);
    AddBind(vs);
    AddBind(fs);

    AddBind(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices, 0));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, perInstanceData.positions, 2));
    AddBind(std::make_shared<Bind::TransformUniformBuffer>(gfx, *this));
    AddBind(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub, 1));
    AddBind(std::make_shared<Bind::RasterizerState>(gfx, RasterizerDesc { FillMode::Solid, CullMode::Back }));
}

LatticeVertex::~LatticeVertex()
{
}

void LatticeVertex::Update(Graphics& gfx)
{
    m_isVisible = (world.renderFlags & RenderFlag_DrawLattice) && (world.renderFlags & RenderFlag_DrawLatticeVertex);

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

glm::mat4 LatticeVertex::GetTransformMatrix() const
{
    return glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.2f);
}
