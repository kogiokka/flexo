#include <utility>
#include <vector>

#include "Vertex.hpp"
#include "World.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/Shader.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"
#include "bindable/VertexLayout.hpp"
#include "drawable/VolumetricModel.hpp"

VolumetricModel::VolumetricModel(Graphics& gfx, Mesh const& mesh)
    : m_count(0)
    , m_ub {}
    , m_texColor(nullptr)
    , m_texPattern(nullptr)
{
    std::vector<AttributeDesc> attrs = {
        { "position", 3, GL_FLOAT, GL_FALSE }, // 0
        { "normal", 3, GL_FLOAT, GL_FALSE }, // 1
        { "textureCoord", 2, GL_FLOAT, GL_FALSE }, // 2
        { "translation", 3, GL_FLOAT, GL_FALSE }, // 3
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
    shader->Attach(ShaderStage::Vert, "shader/VolumetricModel.vert");
    shader->Attach(ShaderStage::Frag, "shader/VolumetricModel.frag");
    shader->Link();

    m_ub.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    m_ub.vert.modelMat = glm::mat4(1.0f);
    m_ub.vert.alpha = world.modelColorAlpha;
    m_ub.vert.viewPos = gfx.GetCameraPosition();
    m_ub.vert.light.position = gfx.GetCameraPosition();
    m_ub.vert.light.ambient = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.vert.light.diffusion = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.vert.light.specular = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.vert.material.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.vert.material.diffusion = glm::vec3(0.6f, 0.6f, 0.6f);
    m_ub.vert.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.vert.material.shininess = 256.0f;
    m_ub.vert.isWatermarked = false;

    float color[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    m_texColor = std::make_shared<Bind::Texture2D>(gfx, color, 1, 1, GL_TEXTURE0);
    auto const& [img, w, h, ch] = world.pattern;
    m_texPattern = std::make_shared<Bind::Texture2D>(gfx, img, w, h, GL_TEXTURE1);

    AddBind(std::make_shared<Bind::VertexLayout>(gfx, attrs));
    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, vertices, 0));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, world.theModel->textureCoords, 2));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, world.theModel->positions, 3));
    AddBind(shader);
    AddBind(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub));
    AddBind(m_texColor);
    AddBind(m_texPattern);
}

void VolumetricModel::Draw(Graphics& gfx) const
{
    Drawable::Draw(gfx);

    glVertexBindingDivisor(2, 1); // FIXME: Move to VertexArray bindable
    glVertexBindingDivisor(3, 1);

    gfx.DrawInstanced(m_count, world.theModel->positions.size());
}

void VolumetricModel::Update(Graphics& gfx)
{
    m_ub.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    m_ub.vert.alpha = world.modelColorAlpha;
    m_ub.vert.viewPos = gfx.GetCameraPosition();
    m_ub.vert.light.position = gfx.GetCameraPosition();
    m_ub.vert.isWatermarked = world.isWatermarked;

    for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
        {
            Bind::VertexBuffer* vb = dynamic_cast<Bind::VertexBuffer*>(it->get());
            if ((vb != nullptr) && (vb->GetStartAttrib() == 2)) {
                vb->Update(world.theModel->textureCoords);
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
