#include <utility>
#include <vector>

#include "Task.hpp"
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
#include "drawable/PolygonalModel.hpp"

PolygonalModel::PolygonalModel(Graphics& gfx, Mesh const& mesh)
{
    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, offsetof(VertexPN, position), GLWRInputClassification_PerVertex, 0 },
        { "normal", GLWRFormat_Float3, 0, offsetof(VertexPN, normal), GLWRInputClassification_PerVertex, 0 },
    };

    std::vector<VertexPN> vertices;
    for (unsigned int i = 0; i < mesh.positions.size(); i++) {
        VertexPN v;
        v.position = mesh.positions[i];
        v.normal = mesh.normals[i];
        vertices.push_back(v);
    }

    m_isVisible = true;

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

    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));

    Task draw;
    draw.mDrawable = this;

    auto pipeline = std::make_shared<Bind::ProgramPipeline>(gfx);
    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/PolygonalModel.vert", *pipeline);
    auto fs = std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/PolygonalModel.frag", *pipeline);
    draw.AddBindable(pipeline);
    draw.AddBindable(vs);
    draw.AddBindable(fs);
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    draw.AddBindable(std::make_shared<Bind::TransformUniformBuffer>(gfx, glm::mat4(1.0f)));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub, 1));
    draw.AddBindable(std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode::GLWRFillMode_Solid, GLWRCullMode::GLWRCullMode_None }));

    AddTask(draw);
}

PolygonalModel::~PolygonalModel()
{
}

void PolygonalModel::Update(Graphics& gfx)
{
    m_ub.frag.alpha = world.modelColorAlpha;
    m_ub.frag.viewPos = gfx.GetCameraPosition();
    m_ub.frag.light.position = world.lightPos;

    // FIXME Need to rework UniformBuffer creation/update
    auto const& taskBinds = m_tasks.front().mBinds;
    for (auto it = taskBinds.begin(); it != taskBinds.end(); it++) {
        Bind::UniformBuffer<UniformBlock>* ub = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
        if (ub != nullptr) {
            ub->Update(m_ub);
        }
    }
}

std::string PolygonalModel::GetName() const
{
    return "Polygonal Model";
}
