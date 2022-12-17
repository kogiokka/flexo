#include "gfx/drawable/MapFace.hpp"
#include "Vertex.hpp"
#include "World.hpp"
#include "gfx/Task.hpp"
#include "gfx/bindable/InputLayout.hpp"
#include "gfx/bindable/Primitive.hpp"
#include "gfx/bindable/RasterizerState.hpp"
#include "gfx/bindable/Sampler.hpp"
#include "gfx/bindable/Texture2D.hpp"
#include "gfx/bindable/TextureManager.hpp"
#include "gfx/bindable/TransformUniformBuffer.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/bindable/VertexBuffer.hpp"
#include "gfx/bindable/program/FragmentShaderProgram.hpp"
#include "gfx/bindable/program/VertexShaderProgram.hpp"

MapFace::MapFace(Graphics& gfx, Mesh const& mesh)
{
    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, offsetof(VertexPNT, position), GLWRInputClassification_PerVertex, 0 },
        { "normal", GLWRFormat_Float3, 0, offsetof(VertexPNT, normal), GLWRInputClassification_PerVertex, 0 },
        { "textureCoord", GLWRFormat_Float2, 0, offsetof(VertexPNT, texcoord), GLWRInputClassification_PerVertex, 0 },
    };

    std::vector<VertexPNT> vertices;
    vertices.reserve(mesh.faces.size() * 3);

    for (auto const& f : mesh.faces) {
        VertexPNT v1, v2, v3;
        glm::vec3 p1, p2, p3;
        glm::vec3 normal;

        p1 = mesh.positions[f.x];
        p2 = mesh.positions[f.y];
        p3 = mesh.positions[f.z];
        normal = glm::cross(p2 - p1, p3 - p1);

        v1.position = mesh.positions[f.x];
        v2.position = mesh.positions[f.y];
        v3.position = mesh.positions[f.z];
        v1.texcoord = mesh.textureCoords[f.x];
        v2.texcoord = mesh.textureCoords[f.y];
        v3.texcoord = mesh.textureCoords[f.z];
        v1.normal = normal;
        v2.normal = normal;
        v3.normal = normal;

        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
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

    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/MapFace.vert");
    draw.AddBindable(vs);
    draw.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/MapFace.frag"));
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    draw.AddBindable(std::make_shared<Bind::TransformUniformBuffer>(gfx, glm::mat4(1.0f)));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub, 1));
    draw.AddBindable(Bind::TextureManager::Resolve(gfx, world.imagePath.c_str(), 0));
    draw.AddBindable(std::make_shared<Bind::Sampler>(gfx, samplerDesc, 0));
    draw.AddBindable(
        std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_None }));

    AddTask(draw);
}

MapFace::~MapFace()
{
}

// FIXME
void MapFace::ChangeTexture(Graphics& gfx, char const* filename)
{
    bool (*const FindTexture)(std::shared_ptr<Bind::Bindable>&)
        = [](std::shared_ptr<Bind::Bindable>& bind) { return (dynamic_cast<Bind::Texture2D*>(bind.get()) != nullptr); };

    auto& taskBinds = m_tasks.front().mBinds;
    taskBinds.erase(std::remove_if(taskBinds.begin(), taskBinds.end(), FindTexture), taskBinds.end());
    m_tasks.front().AddBindable(Bind::TextureManager::Resolve(gfx, filename, 0));
}

void MapFace::Update(Graphics& gfx)
{
    m_ub.frag.viewPos = gfx.GetCameraPosition();
    m_ub.frag.light.position = world.lightPos;

    for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
        Bind::VertexBuffer* vb = dynamic_cast<Bind::VertexBuffer*>(it->get());
        if ((vb != nullptr) && (vb->GetStartAttrib() == 0)) {
            Mesh mesh = world.mapMesh;
            std::vector<VertexPNT> vertices;
            vertices.reserve(mesh.faces.size() * 3);

            for (auto const& f : mesh.faces) {
                VertexPNT v1, v2, v3;
                glm::vec3 p1, p2, p3;
                glm::vec3 normal;

                p1 = mesh.positions[f.x];
                p2 = mesh.positions[f.y];
                p3 = mesh.positions[f.z];
                normal = glm::cross(p2 - p1, p3 - p1);

                v1.position = mesh.positions[f.x];
                v2.position = mesh.positions[f.y];
                v3.position = mesh.positions[f.z];
                v1.texcoord = mesh.textureCoords[f.x];
                v2.texcoord = mesh.textureCoords[f.y];
                v3.texcoord = mesh.textureCoords[f.z];
                v1.normal = normal;
                v2.normal = normal;
                v3.normal = normal;

                vertices.push_back(v1);
                vertices.push_back(v2);
                vertices.push_back(v3);
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
