#include <utility>
#include <vector>

#include "World.hpp"
#include "assetlib/STL/STLImporter.hpp"
#include "gfx/Task.hpp"
#include "gfx/bindable/InputLayout.hpp"
#include "gfx/bindable/Primitive.hpp"
#include "gfx/bindable/RasterizerState.hpp"
#include "gfx/bindable/Sampler.hpp"
#include "gfx/bindable/TextureManager.hpp"
#include "gfx/bindable/TransformUniformBuffer.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/bindable/VertexBuffer.hpp"
#include "gfx/bindable/program/FragmentShaderProgram.hpp"
#include "gfx/bindable/program/VertexShaderProgram.hpp"
#include "gfx/drawable/VolumetricModel.hpp"

VolumetricModel::VolumetricModel(Graphics& gfx, Mesh mesh)
    : m_ublight {}
    , m_ubmat {}
    , m_ubo {}
{
    m_isVisible = true;
    m_ublight.AddElement(UniformBlock::Type::vec3f32, "light.position");
    m_ublight.AddElement(UniformBlock::Type::vec3f32, "light.ambient");
    m_ublight.AddElement(UniformBlock::Type::vec3f32, "light.diffusion");
    m_ublight.AddElement(UniformBlock::Type::vec3f32, "light.specular");

    m_ubmat.AddElement(UniformBlock::Type::vec3f32, "material.ambient");
    m_ubmat.AddElement(UniformBlock::Type::vec3f32, "material.diffusion");
    m_ubmat.AddElement(UniformBlock::Type::vec3f32, "material.specular");
    m_ubmat.AddElement(UniformBlock::Type::f32, "material.shininess");

    m_ubo.AddElement(UniformBlock::Type::vec3f32, "viewPos");

    m_ublight.FinalizeLayout();
    m_ubmat.FinalizeLayout();
    m_ubo.FinalizeLayout();

    m_ublight.Assign("light.position", gfx.GetCameraPosition());
    m_ublight.Assign("light.ambient", glm::vec3(0.8f, 0.8f, 0.8f));
    m_ublight.Assign("light.diffusion", glm::vec3(0.8f, 0.8f, 0.8f));
    m_ublight.Assign("light.specular", glm::vec3(0.8f, 0.8f, 0.8f));

    m_ubmat.Assign("material.ambient", glm::vec3(0.3f, 0.3f, 0.3f));
    m_ubmat.Assign("material.diffusion", glm::vec3(0.6f, 0.6f, 0.6f));
    m_ubmat.Assign("material.specular", glm::vec3(0.3f, 0.3f, 0.3f));
    m_ubmat.Assign("material.shininess", 256.0f);

    m_ubo.Assign("viewPos", gfx.GetCameraPosition());

    GLWRSamplerDesc samplerDesc;
    samplerDesc.coordinateS = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.coordinateT = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.coordinateR = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.filter = GLWRFilter_MinMagNearest_NoMip;

    auto bufp = VertexBuffer(mesh.positions);
    auto bufn = VertexBuffer(mesh.normals);
    auto buft = VertexBuffer(mesh.textureCoords);
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, bufp, 0));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, bufn, 1));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, buft, 2));

    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, bufp.OffsetOfPosition(), GLWRInputClassification_PerVertex, 0 },
        { "normal", GLWRFormat_Float3, 1, bufn.OffsetOfNormal(), GLWRInputClassification_PerVertex, 0 },
        { "textureCoord", GLWRFormat_Float2, 2, buft.OffsetOfTextureCoords(), GLWRInputClassification_PerVertex, 0 },
    };

    Task draw;
    draw.mDrawable = this;

    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/VolumetricModel.vert");
    draw.AddBindable(vs);
    draw.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/VolumetricModel.frag"));
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    draw.AddBindable(std::make_shared<Bind::TransformUniformBuffer>(gfx, glm::mat4(1.0f)));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, m_ublight, 1));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, m_ubmat, 2));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, m_ubo, 3));
    draw.AddBindable(
        std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_Back }));
    draw.AddBindable(Bind::TextureManager::Resolve(gfx, "res/images/blank.png", 0));
    draw.AddBindable(std::make_shared<Bind::Sampler>(gfx, samplerDesc, 0));
    draw.AddBindable(std::make_shared<Bind::Sampler>(gfx, samplerDesc, 1));
    AddTask(draw);
}

VolumetricModel::~VolumetricModel()
{
}

// FIXME
void VolumetricModel::ChangeTexture(Graphics& gfx, char const* filename)
{
    bool (*const FindTexture)(std::shared_ptr<Bind::Bindable>&)
        = [](std::shared_ptr<Bind::Bindable>& bind) { return (dynamic_cast<Bind::Texture2D*>(bind.get()) != nullptr); };

    auto& taskBinds = m_tasks.front().mBinds;
    taskBinds.erase(std::remove_if(taskBinds.begin(), taskBinds.end(), FindTexture), taskBinds.end());
    m_tasks.front().AddBindable(Bind::TextureManager::Resolve(gfx, filename, 0));
}

void VolumetricModel::Update(Graphics& gfx)
{
    m_ublight.Assign("light.position", gfx.GetCameraPosition());
    m_ubo.Assign("viewPos", gfx.GetCameraPosition());

    // FIXME Need to rework UniformBuffer creation/update
    auto const& taskBinds = m_tasks.front().mBinds;
    for (auto it = taskBinds.begin(); it != taskBinds.end(); it++) {
        Bind::UniformBuffer* ub = dynamic_cast<Bind::UniformBuffer*>(it->get());
        if (ub != nullptr) {
            if (ub->Index() == 1) {
                ub->Update(gfx, m_ublight);
            }
            if (ub->Index() == 3) {
                ub->Update(gfx, m_ubo);
            }
        }
    }
}
