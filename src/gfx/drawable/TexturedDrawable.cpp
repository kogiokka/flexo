#include "gfx/drawable/TexturedDrawable.hpp"
#include "World.hpp"
#include "gfx/BindStep.hpp"
#include "gfx/VertexBuffer.hpp"
#include "gfx/bindable/InputLayout.hpp"
#include "gfx/bindable/Primitive.hpp"
#include "gfx/bindable/RasterizerState.hpp"
#include "gfx/bindable/Sampler.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/bindable/VertexBuffer.hpp"
#include "gfx/bindable/program/FragmentShaderProgram.hpp"
#include "gfx/bindable/program/VertexShaderProgram.hpp"

TexturedDrawable::TexturedDrawable(Graphics& gfx, Mesh const& mesh, std::shared_ptr<Bind::Texture2D> texture)
{

    VertexBuffer buf(mesh);

    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, buf));

    m_vertCount = buf.Count();
}

TexturedDrawable::~TexturedDrawable()
{
}
// FIXME
void TexturedDrawable::ChangeTexture(std::shared_ptr<Bind::Texture2D> texture)
{
    // bool (*const FindTexture)(std::shared_ptr<Bind::Bindable>&)
    //     = [](std::shared_ptr<Bind::Bindable>& bind) { return (dynamic_cast<Bind::Texture2D*>(bind.get()) != nullptr); };
    //
    // auto& bindings = m_steps.front().bindables;
    // bindings.erase(std::remove_if(bindings.begin(), bindings.end(), FindTexture), bindings.end());
    // m_steps.front().AddBindable(texture);
}

// void TexturedDrawable::Update(Graphics& gfx)
// {
//     m_ubs["transform"].Assign("viewProj", gfx.GetViewProjectionMatrix());
//     m_ubs["light"].Assign("position", gfx.GetCameraPosition());
//     m_ubs["viewPos"].Assign("viewPos", gfx.GetCameraPosition());
//
//     UpdateUniformBuffers(gfx);
// }
//
