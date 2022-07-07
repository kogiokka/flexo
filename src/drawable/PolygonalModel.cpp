#include <utility>
#include <vector>

#include "Vertex.hpp"
#include "World.hpp"
#include "bindable/InputLayout.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/TransformUniformBuffer.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"
#include "bindable/program/FragmentShaderProgram.hpp"
#include "bindable/program/ProgramPipeline.hpp"
#include "bindable/program/VertexShaderProgram.hpp"
#include "drawable/PolygonalModel.hpp"

PolygonalModel::PolygonalModel(Graphics& gfx, Mesh const& mesh)
{
    std::vector<InputElementDesc> inputs = {
        { "position", InputFormat::Float3, 0, offsetof(VertexPN, position), InputClassification::PerVertex, 0 }, // 0
        { "normal", InputFormat::Float3, 0, offsetof(VertexPN, normal), InputClassification::PerVertex, 0 }, // 1
    };

    std::vector<VertexPN> vertices;
    for (unsigned int i = 0; i < mesh.positions.size(); i++) {
        VertexPN v;
        v.position = mesh.positions[i];
        v.normal = mesh.normals[i];
        vertices.push_back(v);
    }

    m_ub.frag.alpha = world.modelColorAlpha;
    m_ub.frag.viewPos = gfx.GetCameraPosition();
    m_ub.frag.light.position = world.lightPos;
    m_ub.frag.light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    m_ub.frag.light.diffusion = glm::vec3(0.5f, 0.5f, 0.5f);
    m_ub.frag.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    m_ub.frag.material.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    m_ub.frag.material.diffusion = glm::vec3(0.0f, 0.6352941f, 0.9294118f);
    m_ub.frag.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.frag.material.shininess = 32.0f;

    auto pipeline = std::make_shared<Bind::ProgramPipeline>(gfx);
    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/PolygonalModel.vert", pipeline->GetId());
    auto fs = std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/PolygonalModel.frag", pipeline->GetId());
    AddBind(pipeline);
    AddBind(vs);
    AddBind(fs);

    AddBind(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));
    AddBind(std::make_shared<Bind::TransformUniformBuffer>(gfx, *this));
    AddBind(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub, 1));
}

PolygonalModel::~PolygonalModel() { }

void PolygonalModel::Update(Graphics& gfx)
{
    m_isVisible = world.renderFlags & RenderFlag_DrawModel;

    m_ub.frag.alpha = world.modelColorAlpha;
    m_ub.frag.viewPos = gfx.GetCameraPosition();
    m_ub.frag.light.position = world.lightPos;

    for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
        Bind::UniformBuffer<UniformBlock>* buf = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
        if (buf != nullptr) {
            buf->Update(m_ub);
            break;
        }
    }
}

glm::mat4 PolygonalModel::GetTransformMatrix() const
{
    return glm::mat4(1.0f);
}
