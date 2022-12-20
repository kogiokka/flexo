#include <utility>
#include <vector>

#include "World.hpp"
#include "gfx/Task.hpp"
#include "gfx/VertexBuffer.hpp"
#include "gfx/bindable/InputLayout.hpp"
#include "gfx/bindable/Primitive.hpp"
#include "gfx/bindable/RasterizerState.hpp"
#include "gfx/bindable/TransformUniformBuffer.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/bindable/VertexBuffer.hpp"
#include "gfx/bindable/program/FragmentShaderProgram.hpp"
#include "gfx/bindable/program/VertexShaderProgram.hpp"
#include "gfx/drawable/SolidDrawable.hpp"

SolidDrawable::SolidDrawable(Graphics& gfx, Mesh const& mesh)
{
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

    VertexBuffer buf(mesh);
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, buf));

    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, buf.OffsetOfPosition(), GLWRInputClassification_PerVertex, 0 },
        { "normal", GLWRFormat_Float3, 0, buf.OffsetOfNormal(), GLWRInputClassification_PerVertex, 0 },
    };

    Task draw;
    draw.mDrawable = this;

    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/SolidDrawable.vert");
    draw.AddBindable(vs);
    draw.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/SolidDrawable.frag"));
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    draw.AddBindable(std::make_shared<Bind::TransformUniformBuffer>(gfx, glm::mat4(1.0f)));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, m_ublight, 1));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, m_ubmat, 2));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, m_ubo, 3));
    draw.AddBindable(
        std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_None }));

    AddTask(draw);
}

SolidDrawable::~SolidDrawable()
{
}

void SolidDrawable::Update(Graphics& gfx)
{
    m_ubo.Assign("viewPos", gfx.GetCameraPosition());
    m_ublight.Assign("light.position", gfx.GetCameraPosition());

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
