#include <cassert>
#include <utility>
#include <vector>

#include "Wireframe.hpp"
#include "World.hpp"
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

    VertexBuffer bufpos(wireframe.positions);
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_LINES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, bufpos));
    AddBind(std::make_shared<Bind::IndexBuffer>(gfx, indices));

    m_indexCount = indices.size();
}

WireDrawable::~WireDrawable()
{
}
