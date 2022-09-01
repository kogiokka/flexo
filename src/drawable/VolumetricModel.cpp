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
#include "drawable/VolumetricModel.hpp"

VolumetricModel::VolumetricModel(Graphics& gfx, Mesh const& instanceMesh, Mesh const& perInstanceData)
    : m_ub {}
    , m_texColor(nullptr)
    , m_texPattern(nullptr)
{
    std::vector<InputElementDesc> inputs = {
        { "position", InputFormat::Float3, 0, offsetof(VertexPN, position), InputClassification::PerVertex, 0 },
        { "normal", InputFormat::Float3, 0, offsetof(VertexPN, normal), InputClassification::PerVertex, 0 },
        { "textureCoord", InputFormat::Float2, 2, 0, InputClassification::PerInstance, 1 },
        { "translation", InputFormat::Float3, 3, 0, InputClassification::PerInstance, 1 },
    };

    std::vector<VertexPN> vertices;
    for (unsigned int i = 0; i < instanceMesh.positions.size(); i++) {
        VertexPN v;
        v.position = instanceMesh.positions[i];
        v.normal = instanceMesh.normals[i];
        vertices.push_back(v);
    }

    m_isVisible = true;

    m_ub.vert.viewPos = gfx.GetCameraPosition();
    m_ub.vert.light.position = gfx.GetCameraPosition();
    m_ub.vert.light.ambient = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.vert.light.diffusion = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.vert.light.specular = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.vert.material.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.vert.material.diffusion = glm::vec3(0.6f, 0.6f, 0.6f);
    m_ub.vert.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.vert.material.shininess = 256.0f;
    m_ub.vert.isWatermarked = false;

    float color[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    m_texColor = std::make_shared<Bind::Texture2D>(gfx, color, 1, 1, GL_TEXTURE0);
    auto const& [img, w, h, ch] = world.pattern;
    m_texPattern = std::make_shared<Bind::Texture2D>(gfx, img, w, h, GL_TEXTURE1);

    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices, 0));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, perInstanceData.textureCoords, 2));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, perInstanceData.positions, 3));

    Task draw;
    draw.mDrawable = this;

    auto pipeline = std::make_shared<Bind::ProgramPipeline>(gfx);
    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/VolumetricModel.vert", *pipeline);
    auto fs = std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/VolumetricModel.frag", *pipeline);
    draw.AddBindable(pipeline);
    draw.AddBindable(vs);
    draw.AddBindable(fs);
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    draw.AddBindable(std::make_shared<Bind::TransformUniformBuffer>(gfx, glm::mat4(1.0f)));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub, 1));
    draw.AddBindable(std::make_shared<Bind::RasterizerState>(gfx, RasterizerDesc { FillMode::Solid, CullMode::Back }));
    draw.AddBindable(m_texColor);
    draw.AddBindable(m_texPattern);

    AddTask(draw);
}

VolumetricModel::~VolumetricModel()
{
}

void VolumetricModel::Update(Graphics& gfx)
{
    m_ub.vert.viewPos = gfx.GetCameraPosition();
    m_ub.vert.light.position = gfx.GetCameraPosition();
    m_ub.vert.isWatermarked = world.isWatermarked;

    for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
        Bind::VertexBuffer* vb = dynamic_cast<Bind::VertexBuffer*>(it->get());
        if ((vb != nullptr) && (vb->GetStartAttrib() == 2)) {
            vb->Update(world.theModel->textureCoords);
        }
    }

    // FIXME Need to rework UniformBuffer creation/update
    auto const& taskBinds = m_tasks.front().mBinds;
    for (auto it = taskBinds.begin(); it != taskBinds.end(); it++) {
        Bind::UniformBuffer<UniformBlock>* ub = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
        if (ub != nullptr) {
            ub->Update(m_ub);
        }
    }
}

std::string VolumetricModel::GetName() const
{
    return "Volumetric Model";
}
