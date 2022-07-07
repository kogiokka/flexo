#include <utility>
#include <vector>

#include "drawable/LightSource.hpp"

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

LightSource::LightSource(Graphics& gfx, Mesh const& mesh)
{
    std::vector<InputElementDesc> inputs = {
        { "position", InputFormat::Float3, 0, 0, InputClassification::PerVertex, 0 },
    };

    std::vector<VertexPN> vertices;
    for (unsigned int i = 0; i < mesh.positions.size(); i++) {
        VertexPN v;
        v.position = mesh.positions[i];
        v.normal = mesh.normals[i];
        vertices.push_back(v);
    }

    m_ub.frag.lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    auto pipeline = std::make_shared<Bind::ProgramPipeline>(gfx);
    AddBind(std::make_shared<Bind::InputLayout>(gfx, inputs));
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));
    AddBind(pipeline);
    AddBind(std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/LightSource.vert", pipeline->GetId()));
    AddBind(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/LightSource.frag", pipeline->GetId()));
    AddBind(std::make_shared<Bind::TransformUniformBuffer>(gfx, *this));
    AddBind(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub, 1));
}

LightSource::~LightSource() { }

void LightSource::Update(Graphics&)
{
    m_isVisible = world.renderFlags & RenderFlag_DrawLightSource;

    for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
        Bind::UniformBuffer<UniformBlock>* buf = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
        if (buf != nullptr) {
            buf->Update(m_ub);
            break;
        }
    }
}

glm::mat4 LightSource::GetTransformMatrix() const
{
    return glm::translate(glm::mat4(1.0f), world.lightPos) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.2f);
}
