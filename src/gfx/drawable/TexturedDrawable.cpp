#include "gfx/drawable/TexturedDrawable.hpp"
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
    GLWRSamplerDesc samplerDesc;
    samplerDesc.coordinateS = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.coordinateT = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.coordinateR = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.filter = GLWRFilter_MinMagNearest_MipNearest;

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

    m_ubs["transform"].Assign("model", m_transform);
    m_ubs["transform"].Assign("viewProj", gfx.GetViewProjectionMatrix());

    auto const& cam = gfx.GetCamera();
    m_ubs["light"].Assign("position", cam.position + 3.0f * (-cam.basis.sideway + cam.basis.up));
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

    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, buf.OffsetOfPosition(), GLWRInputClassification_PerVertex, 0 },
        { "normal", GLWRFormat_Float3, 0, buf.OffsetOfNormal(), GLWRInputClassification_PerVertex, 0 },
        { "textureCoord", GLWRFormat_Float2, 0, buf.OffsetOfTextureCoords(), GLWRInputClassification_PerVertex, 0 },
    };

    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, buf));

    BindStep step;

    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/TexturedDrawable.vert");
    step.AddBindable(vs);
    step.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/TexturedDrawable.frag"));
    step.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    for (auto const& [id, ub] : m_ubs) {
        step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, ub, id));
    }
    step.AddBindable(texture);
    step.AddBindable(std::make_shared<Bind::Sampler>(gfx, samplerDesc, 0));
    step.AddBindable(
        std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_None }));

    AddBindStep(step);

    m_vertCount = buf.Count();
}

TexturedDrawable::~TexturedDrawable()
{
}

// FIXME
void TexturedDrawable::ChangeTexture(std::shared_ptr<Bind::Texture2D> texture)
{
    bool (*const FindTexture)(std::shared_ptr<Bind::Bindable>&)
        = [](std::shared_ptr<Bind::Bindable>& bind) { return (dynamic_cast<Bind::Texture2D*>(bind.get()) != nullptr); };

    auto& bindings = m_steps.front().bindables;
    bindings.erase(std::remove_if(bindings.begin(), bindings.end(), FindTexture), bindings.end());
    m_steps.front().AddBindable(texture);
}

void TexturedDrawable::Update(Graphics& gfx)
{
    m_ubs["transform"].Assign("model", m_transform);
    m_ubs["transform"].Assign("viewProj", gfx.GetViewProjectionMatrix());
    m_ubs["viewPos"].Assign("viewPos", gfx.GetCameraPosition());

    auto const& cam = gfx.GetCamera();
    m_ubs["light"].Assign("position", cam.position + 3.0f * (-cam.basis.sideway + cam.basis.up));
}
