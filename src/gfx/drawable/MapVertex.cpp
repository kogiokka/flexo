#include <utility>
#include <vector>

#include "World.hpp"
#include "gfx/Task.hpp"
#include "gfx/bindable/InputLayout.hpp"
#include "gfx/bindable/Primitive.hpp"
#include "gfx/bindable/RasterizerState.hpp"
#include "gfx/bindable/TransformUniformBuffer.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/bindable/VertexBuffer.hpp"
#include "gfx/bindable/program/FragmentShaderProgram.hpp"
#include "gfx/bindable/program/VertexShaderProgram.hpp"
#include "gfx/drawable/MapVertex.hpp"

MapVertex::MapVertex(Graphics& gfx, Mesh const& instanceMesh, std::vector<glm::vec3> const& perInstanceData)
    : m_ub {}
{
    m_isVisible = true;

    m_ub.frag.viewPos = gfx.GetCameraPosition();
    m_ub.frag.light.position = gfx.GetCameraPosition();
    m_ub.frag.light.ambient = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.frag.light.diffusion = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.frag.light.specular = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.frag.material.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.frag.material.diffusion = glm::vec3(0.6f, 0.6f, 0.6f);
    m_ub.frag.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.frag.material.shininess = 256.0f;

    VertexBuffer bufInst(instanceMesh);
    VertexBuffer bufPerInst(perInstanceData);

    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, bufInst, 0));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, bufPerInst, 1));

    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, bufInst.OffsetOfPosition(), GLWRInputClassification_PerVertex, 0 },
        { "normal", GLWRFormat_Float3, 0, bufInst.OffsetOfNormal(), GLWRInputClassification_PerVertex, 0 },
        { "translation", GLWRFormat_Float3, 1, bufPerInst.OffsetOfPosition(), GLWRInputClassification_PerInstance, 1 },
    };

    Task draw;
    draw.mDrawable = this;

    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/MapVertex.vert");
    draw.AddBindable(vs);
    draw.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/MapVertex.frag"));
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    draw.AddBindable(
        std::make_shared<Bind::TransformUniformBuffer>(gfx, glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.02f)));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, m_ub, 1));
    draw.AddBindable(
        std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_Back }));

    AddTask(draw);
}

MapVertex::~MapVertex()
{
}

void MapVertex::Update(Graphics& gfx)
{
    m_ub.frag.viewPos = gfx.GetCameraPosition();
    m_ub.frag.light.position = gfx.GetCameraPosition();

    for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
        Bind::VertexBuffer* vb = dynamic_cast<Bind::VertexBuffer*>(it->get());
        if ((vb != nullptr) && (vb->GetStartAttrib() == 1)) {
            vb->Update(gfx, VertexBuffer(world.mapMesh.positions));
        }
    }

    // FIXME Need to rework UniformBuffer creation/update
    auto const& taskBinds = m_tasks.front().mBinds;
    for (auto it = taskBinds.begin(); it != taskBinds.end(); it++) {
        Bind::UniformBuffer* ub = dynamic_cast<Bind::UniformBuffer*>(it->get());
        if (ub != nullptr) {
            ub->Update<UniformBlock>(gfx, m_ub);
        }
    }
}
