#include <utility>
#include <vector>

#include "drawable/LightSource.hpp"

#include "Vertex.hpp"
#include "World.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/Shader.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"
#include "bindable/VertexLayout.hpp"

LightSource::LightSource(Graphics& gfx, Mesh const& mesh)
{
    std::vector<AttributeDesc> attrs = {
        { "position", 3, GL_FLOAT, GL_FALSE },
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
    shader->Attach(ShaderStage::Vert, "shader/LightSource.vert");
    shader->Attach(ShaderStage::Frag, "shader/LightSource.frag");
    shader->Link();

    m_scaling = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.2f);
    m_ub.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    m_ub.vert.modelMat = glm::translate(glm::mat4(1.0f), world.lightPos) * m_scaling;
    m_ub.frag.lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    AddBind(std::make_shared<Bind::VertexLayout>(gfx, attrs));
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices));
    AddBind(shader);
    AddBind(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub));
}

void LightSource::Draw(Graphics& gfx) const
{
    Drawable::Draw(gfx);
    gfx.Draw(m_count);
}

void LightSource::Update(Graphics& gfx)
{
    m_ub.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    m_ub.vert.modelMat = glm::translate(glm::mat4(1.0f), world.lightPos) * m_scaling;

    for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
        Bind::UniformBuffer<UniformBlock>* buf = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
        if (buf != nullptr) {
            buf->Update(m_ub);
            break;
        }
    }
}
