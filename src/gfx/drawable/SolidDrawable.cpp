#include <utility>
#include <vector>

#include "World.hpp"
#include "gfx/BindStep.hpp"
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
    VertexBuffer buf(mesh);
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, buf));

    m_vertCount = buf.Count();
}

SolidDrawable::~SolidDrawable()
{
}
