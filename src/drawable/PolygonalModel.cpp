#include <utility>
#include <vector>

#include "Vertex.hpp"
#include "World.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/Shader.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"
#include "bindable/VertexLayout.hpp"
#include "drawable/PolygonalModel.hpp"

PolygonalModel::PolygonalModel(Graphics& gfx, Mesh const& mesh)
{
    std::vector<AttributeDesc> attrs = {
        { "position", 3, GL_FLOAT, GL_FALSE },
        { "normal", 3, GL_FLOAT, GL_FALSE },
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
    shader->Attach(ShaderStage::Vert, "shader/PolygonalModel.vert");
    shader->Attach(ShaderStage::Frag, "shader/PolygonalModel.frag");
    shader->Link();

    ub_.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    ub_.vert.modelMat = glm::mat4(1.0f);
    ub_.frag.alpha = world.modelColorAlpha;
    ub_.frag.viewPos = gfx.GetCameraPosition();
    ub_.frag.light.position = world.lightPos;
    ub_.frag.light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    ub_.frag.light.diffusion = glm::vec3(0.5f, 0.5f, 0.5f);
    ub_.frag.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    ub_.frag.material.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    ub_.frag.material.diffusion = glm::vec3(0.0f, 0.6352941f, 0.9294118f);
    ub_.frag.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    ub_.frag.material.shininess = 32.0f;

    AddBind(std::make_shared<Bind::VertexLayout>(gfx, attrs));
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));
    AddBind(shader);
    AddBind(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, ub_));
}

void PolygonalModel::Draw(Graphics& gfx) const
{
    Drawable::Draw(gfx);
    gfx.Draw(count_);
}

void PolygonalModel::Update(Graphics& gfx)
{
    ub_.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    ub_.frag.alpha = world.modelColorAlpha;
    ub_.frag.viewPos = gfx.GetCameraPosition();
    ub_.frag.light.position = world.lightPos;

    for (auto it = binds_.begin(); it != binds_.end(); it++) {
        Bind::UniformBuffer<UniformBlock>* buf = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
        if (buf != nullptr) {
            buf->Update(ub_);
            break;
        }
    }
}