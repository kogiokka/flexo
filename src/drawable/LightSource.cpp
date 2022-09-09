#include <utility>
#include <vector>

#include "drawable/LightSource.hpp"

#include "Task.hpp"
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

LightSource::LightSource(Graphics& gfx, Mesh const& mesh)
{
    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, 0, GLWRInputClassification_PerVertex, 0 },
    };

    std::vector<VertexPN> vertices;
    for (unsigned int i = 0; i < mesh.positions.size(); i++) {
        VertexPN v;
        v.position = mesh.positions[i];
        v.normal = mesh.normals[i];
        vertices.push_back(v);
    }

    m_isVisible = false;
    m_ub.frag.lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));

    Task draw;
    draw.mDrawable = this;

    auto pipeline = std::make_shared<Bind::ProgramPipeline>(gfx);
    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/LightSource.vert", *pipeline);
    auto fs = std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/LightSource.frag", *pipeline);
    draw.AddBindable(pipeline);
    draw.AddBindable(vs);
    draw.AddBindable(fs);
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    draw.AddBindable(std::make_shared<Bind::TransformUniformBuffer>(
        gfx, glm::translate(glm::mat4(1.0f), world.lightPos) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.2f))); // FIXME
    draw.AddBindable(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub, 1));
    draw.AddBindable(std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode::GLWRFillMode_Solid, GLWRCullMode::GLWRCullMode_Back }));

    AddTask(draw);
}

LightSource::~LightSource()
{
}

void LightSource::Update(Graphics&)
{
    // FIXME Need to rework UniformBuffer creation/update
    auto const& taskBinds = m_tasks.front().mBinds;
    for (auto it = taskBinds.begin(); it != taskBinds.end(); it++) {
        Bind::UniformBuffer<UniformBlock>* ub = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
        if (ub != nullptr) {
            ub->Update(m_ub);
        }
    }
}

std::string LightSource::GetName() const
{
    return "Light";
}
