#include <utility>
#include <vector>

#include "Vertex.hpp"
#include "VertexArray.hpp"
#include "World.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/ShaderBindable.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"
#include "drawable/LatticeVertex.hpp"

LatticeVertex::LatticeVertex(Graphics& gfx, Mesh const& mesh)
    : count_(0)
    , ub_ {}
{
    std::vector<VertexPN> vertices;

    for (unsigned int i = 0; i < mesh.positions.size(); i++) {
        VertexPN v;
        v.position = mesh.positions[i];
        v.normal = mesh.normals[i];
        vertices.push_back(v);
    }

    count_ = vertices.size();

    ShaderBindable shader(gfx);
    shader.Attach(gfx, ShaderStage::Vert, "shader/LatticeVertex.vert");
    shader.Attach(gfx, ShaderStage::Frag, "shader/LatticeVertex.frag");
    shader.Link(gfx);

    ub_.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    ub_.vert.modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.2f);
    ub_.frag.alpha = world.modelColorAlpha;
    ub_.frag.viewPos = gfx.GetCameraPosition();
    ub_.frag.light.position = gfx.GetCameraPosition();
    ub_.frag.light.ambient = glm::vec3(0.8f, 0.8f, 0.8f);
    ub_.frag.light.diffusion = glm::vec3(0.8f, 0.8f, 0.8f);
    ub_.frag.light.specular = glm::vec3(0.8f, 0.8f, 0.8f);
    ub_.frag.material.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
    ub_.frag.material.diffusion = glm::vec3(0.6f, 0.6f, 0.6f);
    ub_.frag.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    ub_.frag.material.shininess = 256.0f;

    AddBind(std::make_shared<Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<VertexBuffer>(gfx, vertices, VertexAttrib_Position));
    AddBind(std::make_shared<VertexBuffer>(gfx, world.neuronPositions, VertexAttrib_Translation));
    AddBind(std::make_shared<ShaderBindable>(std::move(shader)));
    AddBind(std::make_shared<UniformBuffer<UniformBlock>>(gfx, ub_));
}

void LatticeVertex::Draw(Graphics& gfx) const
{
    Drawable::Draw(gfx);

    glVertexBindingDivisor(VertexAttrib_Translation, 1); // FIXME

    gfx.DrawInstanced(count_, world.neuronPositions.size());
}

void LatticeVertex::Update(Graphics& gfx)
{
    ub_.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    ub_.frag.alpha = world.modelColorAlpha;
    ub_.frag.viewPos = gfx.GetCameraPosition();
    ub_.frag.light.position = gfx.GetCameraPosition();

    for (auto it = binds_.begin(); it != binds_.end(); it++) {
        {
            VertexBuffer* vb = dynamic_cast<VertexBuffer*>(it->get());
            if ((vb != nullptr) && (vb->GetStartAttrib() == VertexAttrib_Translation)) {
                vb->Update(gfx, world.neuronPositions);
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
