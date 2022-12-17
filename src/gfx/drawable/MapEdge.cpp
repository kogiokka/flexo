#include <cassert>
#include <utility>
#include <vector>

#include "Vertex.hpp"
#include "World.hpp"
#include "gfx/Task.hpp"
#include "gfx/bindable/IndexBuffer.hpp"
#include "gfx/bindable/InputLayout.hpp"
#include "gfx/bindable/Primitive.hpp"
#include "gfx/bindable/RasterizerState.hpp"
#include "gfx/bindable/TransformUniformBuffer.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/bindable/VertexBuffer.hpp"
#include "gfx/bindable/program/FragmentShaderProgram.hpp"
#include "gfx/bindable/program/VertexShaderProgram.hpp"
#include "gfx/drawable/MapEdge.hpp"

MapEdge::MapEdge(Graphics& gfx, Mesh const& mesh)
    : m_ub {}
{
    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, 0, GLWRInputClassification_PerVertex, 0 },
    };

    std::vector<glm::vec3> vertices = mesh.positions;
    std::vector<unsigned int> indices;

    auto const& map = *world.theMap;
    int const width = map.size.x;
    int const height = map.size.y;
    for (int i = 0; i < height - 1; ++i) {
        for (int j = 0; j < width - 1; ++j) {
            indices.push_back(i * width + j);
            indices.push_back(i * width + j + 1);
            indices.push_back(i * width + j + 1);
            indices.push_back((i + 1) * width + j + 1);
            indices.push_back((i + 1) * width + j + 1);
            indices.push_back((i + 1) * width + j);
            indices.push_back((i + 1) * width + j);
            indices.push_back(i * width + j);
        }
    }

    m_isVisible = true;

    m_ub.frag.color = glm::vec3(0.7f, 0.7f, 0.7f);

    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_LINES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));
    AddBind(std::make_shared<Bind::IndexBuffer>(gfx, indices));

    Task draw;
    draw.mDrawable = this;

    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/MapEdge.vert");
    draw.AddBindable(vs);
    draw.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/MapEdge.frag"));
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    draw.AddBindable(std::make_shared<Bind::TransformUniformBuffer>(gfx, glm::mat4(1.0f)));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub, 1));
    draw.AddBindable(
        std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_Back }));

    AddTask(draw);
}

MapEdge::~MapEdge()
{
}

void MapEdge::Update(Graphics& gfx)
{
    for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
        Bind::VertexBuffer* vb = dynamic_cast<Bind::VertexBuffer*>(it->get());
        if ((vb != nullptr) && (vb->GetStartAttrib() == 0)) {
            vb->Update(gfx, world.mapMesh.positions);
        }
    }

    // FIXME Need to rework UniformBuffer creation/update
    auto const& taskBinds = m_tasks.front().mBinds;
    for (auto it = taskBinds.begin(); it != taskBinds.end(); it++) {
        Bind::UniformBuffer<UniformBlock>* ub = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
        if (ub != nullptr) {
            ub->Update(gfx, m_ub);
        }
    }
}
