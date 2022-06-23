#include <utility>
#include <vector>

#include "Vertex.hpp"
#include "World.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/Shader.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"
#include "bindable/VertexLayout.hpp"
#include "drawable/LatticeVertex.hpp"

LatticeVertex::LatticeVertex(Graphics& gfx, Mesh const& mesh)
    : count_(0)
    , ub_ {}
{
    std::vector<AttributeDesc> attrs = {
        { "position", 3, GL_FLOAT, GL_FALSE }, // 0
        { "normal", 3, GL_FLOAT, GL_FALSE }, // 1
        { "translation", 3, GL_FLOAT, GL_FALSE }, // 2
    };

    std::vector<VertexPN> vertices;
    for (unsigned int i = 0; i < mesh.positions.size(); i++) {
        VertexPN v;
        v.position = mesh.positions[i];
        v.normal = mesh.normals[i];
        vertices.push_back(v);
    }

    count_ = vertices.size();

    auto shader = std::make_shared<Bind::Shader>(gfx);
    shader->Attach(ShaderStage::Vert, "shader/LatticeVertex.vert");
    shader->Attach(ShaderStage::Frag, "shader/LatticeVertex.frag");
    shader->Link();

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

    AddBind(std::make_shared<Bind::VertexLayout>(gfx, attrs));
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices, 0));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, world.neurons.positions, 2));
    AddBind(shader);
    AddBind(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, ub_));
}

void LatticeVertex::Draw(Graphics& gfx) const
{
    Drawable::Draw(gfx);

    glVertexBindingDivisor(2, 1); // FIXME

    gfx.DrawInstanced(count_, world.neurons.positions.size());
}

void LatticeVertex::Update(Graphics& gfx)
{
    ub_.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    ub_.frag.alpha = world.modelColorAlpha;
    ub_.frag.viewPos = gfx.GetCameraPosition();
    ub_.frag.light.position = gfx.GetCameraPosition();

    for (auto it = binds_.begin(); it != binds_.end(); it++) {
        {
            Bind::VertexBuffer* vb = dynamic_cast<Bind::VertexBuffer*>(it->get());
            if ((vb != nullptr) && (vb->GetStartAttrib() == 2)) {
                vb->Update(world.neurons.positions);
            }
        }
        {
            Bind::UniformBuffer<UniformBlock>* ub = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
            if (ub != nullptr) {
                ub->Update(ub_);
            }
        }
    }
}
