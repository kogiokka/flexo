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

    m_count = vertices.size();

    auto shader = std::make_shared<Bind::Shader>(gfx);
    shader->Attach(ShaderStage::Vert, "shader/PolygonalModel.vert");
    shader->Attach(ShaderStage::Frag, "shader/PolygonalModel.frag");
    shader->Link();

    m_ub.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    m_ub.vert.modelMat = glm::mat4(1.0f);
    m_ub.frag.alpha = world.modelColorAlpha;
    m_ub.frag.viewPos = gfx.GetCameraPosition();
    m_ub.frag.light.position = world.lightPos;
    m_ub.frag.light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    m_ub.frag.light.diffusion = glm::vec3(0.5f, 0.5f, 0.5f);
    m_ub.frag.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    m_ub.frag.material.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    m_ub.frag.material.diffusion = glm::vec3(0.0f, 0.6352941f, 0.9294118f);
    m_ub.frag.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.frag.material.shininess = 32.0f;

    AddBind(std::make_shared<Bind::VertexLayout>(gfx, attrs));
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));
    AddBind(shader);
    AddBind(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub));
}

void PolygonalModel::Draw(Graphics& gfx) const
{
    Drawable::Draw(gfx);
    gfx.Draw(m_count);
}

void PolygonalModel::Update(Graphics& gfx)
{
    m_ub.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    m_ub.frag.alpha = world.modelColorAlpha;
    m_ub.frag.viewPos = gfx.GetCameraPosition();
    m_ub.frag.light.position = world.lightPos;

    for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
        Bind::UniformBuffer<UniformBlock>* buf = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
        if (buf != nullptr) {
            buf->Update(m_ub);
            break;
        }
    }
}
