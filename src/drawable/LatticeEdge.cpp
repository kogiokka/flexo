#include <cassert>
#include <utility>
#include <vector>

#include "Task.hpp"
#include "Vertex.hpp"
#include "World.hpp"
#include "bindable/IndexBuffer.hpp"
#include "bindable/InputLayout.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/RasterizerState.hpp"
#include "bindable/TransformUniformBuffer.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"
#include "bindable/program/FragmentShaderProgram.hpp"
#include "bindable/program/ProgramPipeline.hpp"
#include "bindable/program/VertexShaderProgram.hpp"
#include "drawable/LatticeEdge.hpp"

LatticeEdge::LatticeEdge(Graphics& gfx, Mesh const& mesh, std::vector<unsigned int> const& indices)
    : m_ub {}
{
    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, 0, GLWRInputClassification_PerVertex, 0 },
    };

    std::vector<glm::vec3> vertices = mesh.positions;

    m_isVisible = true;

    m_ub.frag.color = glm::vec3(0.7f, 0.7f, 0.7f);

    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_LINES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));
    AddBind(std::make_shared<Bind::IndexBuffer>(gfx, indices));

    Task draw;
    draw.mDrawable = this;

    auto pipeline = std::make_shared<Bind::ProgramPipeline>(gfx);
    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/LatticeEdge.vert", *pipeline);
    auto fs = std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/LatticeEdge.frag", *pipeline);
    draw.AddBindable(pipeline);
    draw.AddBindable(vs);
    draw.AddBindable(fs);
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    draw.AddBindable(std::make_shared<Bind::TransformUniformBuffer>(gfx, glm::mat4(1.0f)));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub, 1));
    draw.AddBindable(std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode::GLWRFillMode_Solid, GLWRCullMode::GLWRCullMode_Back }));

    AddTask(draw);
}

LatticeEdge::~LatticeEdge()
{
}

void LatticeEdge::Update(Graphics&)
{
    for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
        Bind::VertexBuffer* vb = dynamic_cast<Bind::VertexBuffer*>(it->get());
        if ((vb != nullptr) && (vb->GetStartAttrib() == 0)) {
            vb->Update(world.neurons.positions);
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

std::string LatticeEdge::GetName() const
{
    return "Lattice Edge";
}
