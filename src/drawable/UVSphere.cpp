#include <vector>

#include "Vertex.hpp"
#include "assetlib/STL/STLImporter.hpp"
#include "bindable/ShaderBindable.hpp"
#include "bindable/Uniform.hpp"
#include "bindable/VertexBuffer.hpp"
#include "drawable/UVSphere.hpp"

UVSphere::UVSphere(Graphics& gfx)
{
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
}

void UVSphere::Draw(Graphics& gfx) const
{
    Drawable::Draw(gfx);
    gfx.Draw(count_);
}
