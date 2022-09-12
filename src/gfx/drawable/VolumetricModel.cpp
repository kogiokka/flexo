#include <utility>
#include <vector>

#include "Task.hpp"
#include "Vertex.hpp"
#include "World.hpp"
#include "gfx/bindable/InputLayout.hpp"
#include "gfx/bindable/Primitive.hpp"
#include "gfx/bindable/RasterizerState.hpp"
#include "gfx/bindable/Sampler.hpp"
#include "gfx/bindable/TransformUniformBuffer.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/bindable/VertexBuffer.hpp"
#include "gfx/bindable/program/FragmentShaderProgram.hpp"
#include "gfx/bindable/program/VertexShaderProgram.hpp"
#include "gfx/drawable/VolumetricModel.hpp"

VolumetricModel::VolumetricModel(Graphics& gfx, Mesh const& instanceMesh, Mesh const& perInstanceData)
    : m_ub {}
{
    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, offsetof(VertexPN, position), GLWRInputClassification_PerVertex, 0 },
        { "normal", GLWRFormat_Float3, 0, offsetof(VertexPN, normal), GLWRInputClassification_PerVertex, 0 },
        { "textureCoord", GLWRFormat_Float2, 2, 0, GLWRInputClassification_PerInstance, 1 },
        { "translation", GLWRFormat_Float3, 3, 0, GLWRInputClassification_PerInstance, 1 },
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

    GLWRSamplerDesc samplerDesc;
    samplerDesc.coordinateS = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.coordinateT = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.coordinateR = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.filter = GLWRFilter_MinMagNearest_NoMip;

    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices, 0));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, perInstanceData.textureCoords, 2));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, perInstanceData.positions, 3));

    Task draw;
    draw.mDrawable = this;

    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/VolumetricModel.vert");
    draw.AddBindable(vs);
    draw.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/VolumetricModel.frag"));
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    draw.AddBindable(std::make_shared<Bind::TransformUniformBuffer>(gfx, glm::mat4(1.0f)));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub, 1));
    draw.AddBindable(std::make_shared<Bind::RasterizerState>(
        gfx, GLWRRasterizerDesc { GLWRFillMode::GLWRFillMode_Solid, GLWRCullMode::GLWRCullMode_Back }));
    draw.AddBindable(std::make_shared<Bind::Texture2D>(gfx, color, 1, 1, 0));
    draw.AddBindable(std::make_shared<Bind::Texture2D>(gfx, "res/images/mandala.png", 1));
    draw.AddBindable(std::make_shared<Bind::Sampler>(gfx, samplerDesc, 0));
    draw.AddBindable(std::make_shared<Bind::Sampler>(gfx, samplerDesc, 1));

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
