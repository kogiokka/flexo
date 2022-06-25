#include "drawable/LatticeFace.hpp"
#include "Vertex.hpp"
#include "World.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/Shader.hpp"
#include "bindable/Texture2D.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"
#include "bindable/VertexLayout.hpp"

LatticeFace::LatticeFace(Graphics& gfx, Mesh const& mesh)
{
    std::vector<AttributeDesc> attrs = {
        { "position", 3, GL_FLOAT, GL_FALSE },
        { "normal", 3, GL_FLOAT, GL_FALSE },
        { "textureCoord", 2, GL_FLOAT, GL_FALSE },
    };

    std::vector<VertexPNT> vertices;
    for (unsigned int i = 0; i < mesh.positions.size(); i++) {
        VertexPNT v;
        v.position = mesh.positions[i];
        v.normal = mesh.normals[i];
        v.texcoord = mesh.textureCoords[i];
        vertices.push_back(v);
    }

    m_count = vertices.size();

    auto shader = std::make_shared<Bind::Shader>(gfx);
    shader->Attach(ShaderStage::Vert, "shader/LatticeFace.vert");
    shader->Attach(ShaderStage::Frag, "shader/LatticeFace.frag");
    shader->Link();

    m_ub.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    m_ub.vert.modelMat = glm::mat4(1.0f);
    m_ub.frag.viewPos = gfx.GetCameraPosition();
    m_ub.frag.light.position = world.lightPos;
    m_ub.frag.light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    m_ub.frag.light.diffusion = glm::vec3(0.5f, 0.4f, 0.5f);
    m_ub.frag.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    m_ub.frag.material.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    m_ub.frag.material.diffusion = glm::vec3(1.0f, 1.0f, 1.0f);
    m_ub.frag.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.frag.material.shininess = 32.0f;

    auto const& [img, w, h, ch] = world.pattern;

    AddBind(std::make_shared<Bind::VertexLayout>(gfx, attrs));
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));
    AddBind(shader);
    AddBind(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub));
    AddBind(std::make_shared<Bind::Texture2D>(gfx, img, w, h, GL_TEXTURE0));
}

void LatticeFace::Draw(Graphics& gfx) const
{
    Drawable::Draw(gfx);
    gfx.Draw(m_count);
}

void LatticeFace::Update(Graphics& gfx)
{
    m_ub.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    m_ub.frag.viewPos = gfx.GetCameraPosition();
    m_ub.frag.light.position = world.lightPos;

    for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
        {
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
                vb->Update(vertices);
            }
        }
        {
            Bind::UniformBuffer<UniformBlock>* ub = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
            if (ub != nullptr) {
                ub->Update(m_ub);
            }
        }
    }
}
