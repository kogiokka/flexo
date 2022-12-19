#include <utility>
#include <vector>

#include "gfx/drawable/LightSource.hpp"

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

LightSource::LightSource(Graphics& gfx, Mesh const& mesh)
{
    m_isVisible = false;
    m_ub.frag.lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    VertexBuffer buf(mesh);
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, buf));

    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, buf.OffsetOfPosition(), GLWRInputClassification_PerVertex, 0 },
    };

    Task draw;
    draw.mDrawable = this;

    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/LightSource.vert");
    draw.AddBindable(vs);
    draw.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/LightSource.frag"));
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    draw.AddBindable(std::make_shared<Bind::TransformUniformBuffer>(
        gfx,
        glm::translate(glm::mat4(1.0f), world.lightPos)
            * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.2f))); // FIXME
    draw.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, m_ub, 1));
    draw.AddBindable(
        std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_Back }));

    AddTask(draw);
}

LightSource::~LightSource()
{
}

void LightSource::Update(Graphics& gfx)
{
    // FIXME Need to rework UniformBuffer creation/update
    auto const& taskBinds = m_tasks.front().mBinds;
    for (auto it = taskBinds.begin(); it != taskBinds.end(); it++) {
        Bind::UniformBuffer* ub = dynamic_cast<Bind::UniformBuffer*>(it->get());
        if (ub != nullptr) {
            ub->Update(gfx, m_ub);
        }
    }
}
