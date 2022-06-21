#include <utility>
#include <vector>

#include "Vertex.hpp"
#include "VertexArray.hpp"
#include "World.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/ShaderBindable.hpp"
#include "bindable/UniformBuffer.hpp"
#include "bindable/VertexBuffer.hpp"
#include "drawable/VolumetricModel.hpp"

VolumetricModel::VolumetricModel(Graphics& gfx, Mesh const& mesh)
    : count_(0)
    , ub_ {}
    , texColor_(nullptr)
    , texPattern_(nullptr)
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
    shader.Attach(gfx, ShaderStage::Vert, "shader/VolumetricModel.vert");
    shader.Attach(gfx, ShaderStage::Frag, "shader/VolumetricModel.frag");
    shader.Link(gfx);

    ub_.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    ub_.vert.modelMat = glm::mat4(1.0f);
    ub_.vert.alpha = world.modelColorAlpha;
    ub_.vert.viewPos = gfx.GetCameraPosition();
    ub_.vert.light.position = gfx.GetCameraPosition();
    ub_.vert.light.ambient = glm::vec3(0.8f, 0.8f, 0.8f);
    ub_.vert.light.diffusion = glm::vec3(0.8f, 0.8f, 0.8f);
    ub_.vert.light.specular = glm::vec3(0.8f, 0.8f, 0.8f);
    ub_.vert.material.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
    ub_.vert.material.diffusion = glm::vec3(0.6f, 0.6f, 0.6f);
    ub_.vert.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    ub_.vert.material.shininess = 256.0f;

    auto const& [img, w, h, ch] = world.pattern;
    texPattern_ = std::make_shared<Texture2D>(gfx, img, w, h);

    float color[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    texColor_ = std::make_shared<Texture2D>(gfx, color, 1, 1);

    AddBind(std::make_shared<Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<VertexBuffer>(gfx, vertices, VertexAttrib_Position));
    AddBind(std::make_shared<VertexBuffer>(gfx, world.volModel->positions, VertexAttrib_Translation));
    AddBind(std::make_shared<VertexBuffer>(gfx, world.volModel->textureCoords, VertexAttrib_TextureCoord));
    AddBind(std::make_shared<ShaderBindable>(std::move(shader)));
    AddBind(std::make_shared<UniformBuffer<UniformBlock>>(gfx, ub_));
    AddBind(texColor_);
}

void VolumetricModel::Draw(Graphics& gfx) const
{
    Drawable::Draw(gfx);
    glVertexBindingDivisor(2, 1); // FIXME: Move to VertexArray bindable
    glVertexBindingDivisor(3, 1);

    gfx.DrawInstanced(count_, world.volModel->positions.size());
}

void VolumetricModel::Update(Graphics& gfx)
{
    ub_.vert.viewProjMat = gfx.GetViewProjectionMatrix();
    ub_.vert.alpha = world.modelColorAlpha;
    ub_.vert.viewPos = gfx.GetCameraPosition();
    ub_.vert.light.position = gfx.GetCameraPosition();

    for (auto it = binds_.begin(); it != binds_.end(); it++) {
        UniformBuffer<UniformBlock>* buf = dynamic_cast<UniformBuffer<UniformBlock>*>(it->get());
        if (buf != nullptr) {
            buf->Update(gfx, ub_);
            break;
        }
    }
}
