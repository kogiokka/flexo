#include <vector>

#include <glm/gtc/type_ptr.hpp>

#include "Vertex.hpp"
#include "assetlib/STL/STLImporter.hpp"
#include "bindable/ShaderBindable.hpp"
#include "bindable/Uniform.hpp"
#include "bindable/VertexBuffer.hpp"
#include "drawable/UVSphere.hpp"

UVSphere::UVSphere(Graphics& gfx)
{
    using std::make_unique;

    STLImporter stlImp;
    std::vector<VertexPN> vertices;

    auto cubeMesh = stlImp.ReadFile("res/models/cube.stl");
    for (unsigned int i = 0; i < cubeMesh.positions.size(); i++) {
        VertexPN v;
        v.position = cubeMesh.positions[i];
        v.normal = cubeMesh.normals[i];
        vertices.push_back(v);
    }

    count_ = vertices.size();

    AddBind(std::make_unique<VertexBuffer>(gfx, vertices));

    auto shader = std::make_unique<ShaderBindable>(gfx);
    shader->Attach(gfx, ShaderStage::Vert, "shader/default.vert");
    shader->Attach(gfx, ShaderStage::Frag, "shader/default.frag");
    shader->Link(gfx);

    AddBind(std::move(shader));

    using namespace glm;
    AddBind(make_unique<Uniform<float, 4, 4>>(gfx, "viewProjMat", value_ptr(uniformBuf_.viewProjMat)));
    AddBind(make_unique<Uniform<float, 4, 4>>(gfx, "modelMat", value_ptr(uniformBuf_.modelMat)));
    AddBind(make_unique<Uniform<float, 3, 1>>(gfx, "viewPos", value_ptr(uniformBuf_.viewPos)));
    AddBind(make_unique<Uniform<float, 3, 1>>(gfx, "light.ambient", value_ptr(uniformBuf_.light.ambient)));
    AddBind(make_unique<Uniform<float, 3, 1>>(gfx, "light.diffusion", value_ptr(uniformBuf_.light.diffusion)));
    AddBind(make_unique<Uniform<float, 3, 1>>(gfx, "light.specular", value_ptr(uniformBuf_.light.specular)));
    AddBind(make_unique<Uniform<float, 3, 1>>(gfx, "light.position", value_ptr(uniformBuf_.light.position)));
    AddBind(make_unique<Uniform<float, 3, 1>>(gfx, "material.ambient", value_ptr(uniformBuf_.material.ambient)));
    AddBind(make_unique<Uniform<float, 3, 1>>(gfx, "material.diffusion", value_ptr(uniformBuf_.material.diffusion)));
    AddBind(make_unique<Uniform<float, 3, 1>>(gfx, "material.specular", value_ptr(uniformBuf_.material.specular)));
    AddBind(make_unique<Uniform<float, 1, 1>>(gfx, "material.shininess", &uniformBuf_.material.shininess));
    AddBind(make_unique<Uniform<float, 1, 1>>(gfx, "alpha", &uniformBuf_.alpha));
}

void UVSphere::Draw(Graphics& gfx) const
{
    Drawable::Draw(gfx);
    gfx.Draw(count_);
}
