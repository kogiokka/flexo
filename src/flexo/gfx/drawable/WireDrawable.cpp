#include <cassert>
#include <utility>
#include <vector>

#include "Wireframe.hpp"
#include "gfx/DrawTask.hpp"
#include "gfx/Graphics.hpp"
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

    m_ubs["transform"].Assign("model", m_transform);
    m_ubs["transform"].Assign("viewProj", gfx.GetViewProjectionMatrix());
    m_ubs["color"].Assign("color", glm::vec3(0.7f, 0.7f, 0.7f));

    VertexLayout layout;
    layout.AddAttrib("Position", VertexLayout::AttribFormat::Float3);

    VertexArray vertices(layout);
    for (auto const& p : wireframe.positions) {
        vertices.Assign("Position", p);
        vertices.PushBack();
    }

    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, vertices.GetLayout().GetOffset("Position"), GLWRInputClassification_PerVertex, 0 },
    };

    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_LINES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));
    AddBind(std::make_shared<Bind::IndexBuffer>(gfx, indices));

    BindStep step;
    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/WireDrawable.vert");
    step.AddBindable(vs);
    step.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/WireDrawable.frag"));
    step.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    for (auto const& [id, ub] : m_ubs) {
        step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, ub, id));
    }
    step.AddBindable(
        std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_Back }));
    AddBindStep(step);

    m_indexCount = indices.size();
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
    m_ubs["transform"].Assign("model", m_transform);
    m_ubs["transform"].Assign("viewProj", gfx.GetViewProjectionMatrix());
}
