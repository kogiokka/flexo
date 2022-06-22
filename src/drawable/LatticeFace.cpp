#include "drawable/LatticeFace.hpp"
#include "Vertex.hpp"
#include "World.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/ShaderBindable.hpp"
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

    count_ = vertices.size();

    ShaderBindable shader(gfx);
    shader.Attach(gfx, ShaderStage::Vert, "shader/LatticeFace.vert");
    shader.Attach(gfx, ShaderStage::Frag, "shader/LatticeFace.frag");
    shader.Link(gfx);

    ub_.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    ub_.vert.modelMat = glm::mat4(1.0f);
    ub_.frag.viewPos = gfx.GetCameraPosition();
    ub_.frag.light.position = world.lightPos;
    ub_.frag.light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    ub_.frag.light.diffusion = glm::vec3(0.5f, 0.4f, 0.5f);
    ub_.frag.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    ub_.frag.material.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    ub_.frag.material.diffusion = glm::vec3(1.0f, 1.0f, 1.0f);
    ub_.frag.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    ub_.frag.material.shininess = 32.0f;

    auto const& [img, w, h, ch] = world.pattern;

    AddBind(std::make_shared<VertexLayout>(gfx, attrs));
    AddBind(std::make_shared<Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<VertexBuffer>(gfx, vertices));
    AddBind(std::make_shared<ShaderBindable>(std::move(shader)));
    AddBind(std::make_shared<UniformBuffer<UniformBlock>>(gfx, ub_));
    AddBind(std::make_shared<Texture2D>(gfx, img, w, h, GL_TEXTURE0));
}

void LatticeFace::Draw(Graphics& gfx) const
{
    Drawable::Draw(gfx);
    gfx.Draw(count_);
}

void LatticeFace::Update(Graphics& gfx)
{
    ub_.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    ub_.frag.viewPos = gfx.GetCameraPosition();
    ub_.frag.light.position = world.lightPos;

    for (auto it = binds_.begin(); it != binds_.end(); it++) {
        {
            VertexBuffer* vb = dynamic_cast<VertexBuffer*>(it->get());
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
        {
            UniformBuffer<UniformBlock>* ub = dynamic_cast<UniformBuffer<UniformBlock>*>(it->get());
            if (ub != nullptr) {
                ub->Update(gfx, ub_);
            }
        }
    }
}
