#include <utility>
#include <vector>

#include "World.hpp"
#include "gfx/Task.hpp"
#include "gfx/VertexBuffer.hpp"
#include "gfx/bindable/InputLayout.hpp"
#include "gfx/bindable/Primitive.hpp"
#include "gfx/bindable/RasterizerState.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/bindable/VertexBuffer.hpp"
#include "gfx/bindable/program/FragmentShaderProgram.hpp"
#include "gfx/bindable/program/VertexShaderProgram.hpp"
#include "gfx/drawable/SolidDrawable.hpp"

SolidDrawable::SolidDrawable(Graphics& gfx, Mesh const& mesh)
{
    m_ubs["transform"].AddElement(UniformBlock::Type::mat4, "model");
    m_ubs["transform"].AddElement(UniformBlock::Type::mat4, "viewProj");
    m_ubs["light"].AddElement(UniformBlock::Type::vec3f32, "position");
    m_ubs["light"].AddElement(UniformBlock::Type::vec3f32, "ambient");
    m_ubs["light"].AddElement(UniformBlock::Type::vec3f32, "diffusion");
    m_ubs["light"].AddElement(UniformBlock::Type::vec3f32, "specular");
    m_ubs["material"].AddElement(UniformBlock::Type::vec3f32, "ambient");
    m_ubs["material"].AddElement(UniformBlock::Type::vec3f32, "diffusion");
    m_ubs["material"].AddElement(UniformBlock::Type::vec3f32, "specular");
    m_ubs["material"].AddElement(UniformBlock::Type::f32, "shininess");
    m_ubs["viewPos"].AddElement(UniformBlock::Type::vec3f32, "viewPos");

    m_ubs["transform"].FinalizeLayout();
    m_ubs["light"].FinalizeLayout();
    m_ubs["material"].FinalizeLayout();
    m_ubs["viewPos"].FinalizeLayout();

    m_ubs["transform"].Assign("model", glm::mat4(1.0f));
    m_ubs["transform"].Assign("viewProj", gfx.GetViewProjectionMatrix());
    m_ubs["light"].Assign("position", gfx.GetCameraPosition());
    m_ubs["light"].Assign("ambient", glm::vec3(0.8f, 0.8f, 0.8f));
    m_ubs["light"].Assign("diffusion", glm::vec3(0.8f, 0.8f, 0.8f));
    m_ubs["light"].Assign("specular", glm::vec3(0.8f, 0.8f, 0.8f));
    m_ubs["material"].Assign("ambient", glm::vec3(0.3f, 0.3f, 0.3f));
    m_ubs["material"].Assign("diffusion", glm::vec3(0.6f, 0.6f, 0.6f));
    m_ubs["material"].Assign("specular", glm::vec3(0.3f, 0.3f, 0.3f));
    m_ubs["material"].Assign("shininess", 256.0f);
    m_ubs["viewPos"].Assign("viewPos", gfx.GetCameraPosition());

    m_ubs["transform"].SetBIndex(0);
    m_ubs["light"].SetBIndex(1);
    m_ubs["material"].SetBIndex(2);
    m_ubs["viewPos"].SetBIndex(3);

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
    for (auto const& [id, ub] : m_ubs) {
        draw.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, ub, id));
    }
    draw.AddBindable(
        std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_None }));

    AddTask(draw);
}

SolidDrawable::~SolidDrawable()
{
}

void SolidDrawable::Update(Graphics& gfx)
{
    m_ubs["transform"].Assign("viewProj", gfx.GetViewProjectionMatrix());
    m_ubs["viewPos"].Assign("viewPos", gfx.GetCameraPosition());
    m_ubs["light"].Assign("position", gfx.GetCameraPosition());

    // FIXME Need to rework UniformBuffer creation/update
    auto const& taskBinds = m_tasks.front().mBinds;
    for (auto it = taskBinds.begin(); it != taskBinds.end(); it++) {
        Bind::UniformBuffer* ub = dynamic_cast<Bind::UniformBuffer*>(it->get());
        if (ub != nullptr) {
            ub->Update(gfx, m_ubs[ub->Id()]);
        }
    }
}
