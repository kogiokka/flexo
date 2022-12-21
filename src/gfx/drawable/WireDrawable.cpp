#include <cassert>
#include <utility>
#include <vector>

#include "World.hpp"
#include "gfx/Task.hpp"
#include "gfx/bindable/IndexBuffer.hpp"
#include "gfx/bindable/InputLayout.hpp"
#include "gfx/bindable/Primitive.hpp"
#include "gfx/bindable/RasterizerState.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/bindable/VertexBuffer.hpp"
#include "gfx/bindable/program/FragmentShaderProgram.hpp"
#include "gfx/bindable/program/VertexShaderProgram.hpp"
#include "gfx/drawable/WireDrawable.hpp"

WireDrawable::WireDrawable(Graphics& gfx, Wireframe const& wireframe)
    : m_ubs()
{
    std::vector<unsigned int> indices;

    for (auto const& edge : wireframe.edges) {
        indices.push_back(edge.x);
        indices.push_back(edge.y);
    }

    m_ubs["transform"].AddElement(UniformBlock::Type::mat4, "model");
    m_ubs["transform"].AddElement(UniformBlock::Type::mat4, "viewProj");
    m_ubs["transform"].SetBIndex(0);
    m_ubs["transform"].FinalizeLayout();
    m_ubs["color"].AddElement(UniformBlock::Type::vec3f32, "color");
    m_ubs["color"].FinalizeLayout();
    m_ubs["color"].SetBIndex(1);

    m_ubs["transform"].Assign("model", glm::mat4(1.0f));
    m_ubs["transform"].Assign("viewProj", gfx.GetViewProjectionMatrix());
    m_ubs["color"].Assign("color", glm::vec3(0.7f, 0.7f, 0.7f));

    VertexBuffer bufpos(wireframe.positions);
    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, bufpos.OffsetOfPosition(), GLWRInputClassification_PerVertex, 0 },
    };

    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_LINES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, bufpos));
    AddBind(std::make_shared<Bind::IndexBuffer>(gfx, indices));

    Task draw;
    draw.mDrawable = this;

    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/WireDrawable.vert");
    draw.AddBindable(vs);
    draw.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/WireDrawable.frag"));
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    for (auto const& [id, ub] : m_ubs) {
        draw.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, ub, id));
    }
    draw.AddBindable(
        std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_Back }));

    AddTask(draw);
}

WireDrawable::~WireDrawable()
{
}

void WireDrawable::SetColor(float r, float g, float b)
{
    m_ubs["color"].Assign("color", glm::vec3(r, g, b));
}

void WireDrawable::Update(Graphics& gfx)
{
    m_ubs["transform"].Assign("viewProj", gfx.GetViewProjectionMatrix());

    // FIXME Need to rework UniformBuffer creation/update
    auto const& taskBinds = m_tasks.front().mBinds;
    for (auto it = taskBinds.begin(); it != taskBinds.end(); it++) {
        Bind::UniformBuffer* ub = dynamic_cast<Bind::UniformBuffer*>(it->get());
        if (ub != nullptr) {
            ub->Update(gfx, m_ubs[ub->Id()]);
        }
    }
}
