#include "Mesh.hpp"

#include <cstddef>
#include <string>

bool Mesh::HasPositions() const
{
    return !positions.empty();
}

bool Mesh::HasNormals() const
{
    return !normals.empty();
}

bool Mesh::HasTextureCoords() const
{
    return !textureCoords.empty();
}

std::vector<Vertex> GenVertexArray(Mesh const& mesh)
{
    std::vector<Vertex> vertices;

    Vertex v;
    if (mesh.HasPositions()) {
        v.AddAttrib("Position", Vertex::Format::vec3f32);
    }
    if (mesh.HasNormals()) {
        v.AddAttrib("Normal", Vertex::Format::vec3f32);
    }
    if (mesh.HasTextureCoords()) {
        v.AddAttrib("TexCoord", Vertex::Format::vec2f32);
    }
    v.FinalizeLayout();

    for (unsigned int i = 0; i < mesh.positions.size(); i++) {
        if (mesh.HasPositions()) {
            v.Assign("Position", mesh.positions[i]);
        }
        if (mesh.HasNormals()) {
            v.Assign("Normal", mesh.normals[i]);
        }
        if (mesh.HasTextureCoords()) {
            v.Assign("TexCoord", mesh.textureCoords[i]);
        }
        vertices.push_back(v);
    }

    return vertices;
}
