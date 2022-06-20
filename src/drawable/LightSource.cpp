#include <utility>
#include <vector>

#include "drawable/LightSource.hpp"

#include "Vertex.hpp"
#include "World.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/ShaderBindable.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"

LightSource::LightSource(Graphics& gfx, Mesh const& mesh)
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
    shader.Attach(gfx, ShaderStage::Vert, "shader/LightSource.vert");
    shader.Attach(gfx, ShaderStage::Frag, "shader/LightSource.frag");
    shader.Link(gfx);

    scaling_ = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.2f);
    ub_.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    ub_.vert.modelMat = glm::translate(glm::mat4(1.0f), world.lightPos) * scaling_;
    ub_.frag.lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    AddBind(std::make_shared<Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<VertexBuffer>(gfx, vertices));
    AddBind(std::make_shared<ShaderBindable>(std::move(shader)));
    AddBind(std::make_shared<UniformBuffer<UniformBlock>>(gfx, ub_));
}

void LightSource::Draw(Graphics& gfx) const
{
    Drawable::Draw(gfx);
    gfx.Draw(count_);
}

void LightSource::Update(Graphics& gfx)
{
    ub_.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    ub_.vert.modelMat = glm::translate(glm::mat4(1.0f), world.lightPos) * scaling_;

    for (auto it = binds_.begin(); it != binds_.end(); it++) {
        UniformBuffer<UniformBlock>* buf = dynamic_cast<UniformBuffer<UniformBlock>*>(it->get());
        if (buf != nullptr) {
            buf->Update(gfx, ub_);
            break;
        }
    }
}
