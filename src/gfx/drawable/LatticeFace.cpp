#include "gfx/drawable/LatticeFace.hpp"
#include "Task.hpp"
#include "Vertex.hpp"
#include "World.hpp"
#include "gfx/bindable/InputLayout.hpp"
#include "gfx/bindable/Primitive.hpp"
#include "gfx/bindable/RasterizerState.hpp"
#include "gfx/bindable/Sampler.hpp"
#include "gfx/bindable/Texture2D.hpp"
#include "gfx/bindable/TransformUniformBuffer.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/bindable/VertexBuffer.hpp"
#include "gfx/bindable/program/FragmentShaderProgram.hpp"
#include "gfx/bindable/program/VertexShaderProgram.hpp"

LatticeFace::LatticeFace(Graphics& gfx, Mesh const& mesh)
{
    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, offsetof(VertexPNT, position), GLWRInputClassification_PerVertex, 0 },
        { "normal", GLWRFormat_Float3, 0, offsetof(VertexPNT, normal), GLWRInputClassification_PerVertex, 0 },
        { "textureCoord", GLWRFormat_Float2, 0, offsetof(VertexPNT, texcoord), GLWRInputClassification_PerVertex, 0 },
    };

    std::vector<VertexPNT> vertices;
    for (unsigned int i = 0; i < mesh.positions.size(); i++) {
        VertexPNT v;
        v.position = mesh.positions[i];
        v.normal = mesh.normals[i];
        v.texcoord = mesh.textureCoords[i];
        vertices.push_back(v);
    }

    m_isVisible = false;

    GLWRSamplerDesc samplerDesc;
    samplerDesc.coordinateS = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.coordinateT = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.coordinateR = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.filter = GLWRFilter_MinMagNearest_MipNearest;

    m_ub.frag.viewPos = gfx.GetCameraPosition();
    m_ub.frag.light.position = world.lightPos;
    m_ub.frag.light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    m_ub.frag.light.diffusion = glm::vec3(0.5f, 0.4f, 0.5f);
    m_ub.frag.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    m_ub.frag.material.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    m_ub.frag.material.diffusion = glm::vec3(1.0f, 1.0f, 1.0f);
    m_ub.frag.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.frag.material.shininess = 32.0f;

    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));

    Task draw;
    draw.mDrawable = this;

    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/LatticeFace.vert");
    draw.AddBindable(vs);
    draw.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/LatticeFace.frag"));
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    draw.AddBindable(std::make_shared<Bind::TransformUniformBuffer>(gfx, glm::mat4(1.0f)));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub, 1));
    draw.AddBindable(std::make_shared<Bind::Texture2D>(gfx, "res/images/mandala.png", 0));
    draw.AddBindable(std::make_shared<Bind::Sampler>(gfx, samplerDesc, 0));
    draw.AddBindable(std::make_shared<Bind::RasterizerState>(
        gfx, GLWRRasterizerDesc { GLWRFillMode::GLWRFillMode_Solid, GLWRCullMode::GLWRCullMode_None }));

    AddTask(draw);
}

LatticeFace::~LatticeFace()
{
}

void LatticeFace::Update(Graphics& gfx)
{
    m_ub.frag.viewPos = gfx.GetCameraPosition();
    m_ub.frag.light.position = world.lightPos;

    for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
        Bind::VertexBuffer* vb = dynamic_cast<Bind::VertexBuffer*>(it->get());
        if ((vb != nullptr) && (vb->GetStartAttrib() == 0)) {
            std::vector<VertexPNT> vertices;
            vertices.resize(world.latticeMesh.positions.size());
            for (unsigned int i = 0; i < world.latticeMesh.positions.size(); i++) {
                VertexPNT v;
                v.position = world.latticeMesh.positions[i];
                v.normal = world.latticeMesh.normals[i];
                v.texcoord = world.latticeMesh.textureCoords[i];
                vertices[i] = v;
            }
            vb->Update(gfx, vertices);
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

std::string LatticeFace::GetName() const
{
    return "Lattice Face";
}
