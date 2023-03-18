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

VertexArray GenVertexArray(Mesh const& mesh)
{
    VertexLayout layout;
    if (mesh.HasPositions()) {
        layout.AddAttrib("Position", VertexLayout::AttribFormat::Float3);
    }
    if (mesh.HasNormals()) {
        layout.AddAttrib("Normal", VertexLayout::AttribFormat::Float3);
    }
    if (mesh.HasTextureCoords()) {
        layout.AddAttrib("TexCoord", VertexLayout::AttribFormat::Float2);
    }

    VertexArray arr(layout);
    for (unsigned int i = 0; i < mesh.positions.size(); i++) {
        if (mesh.HasPositions()) {
            arr.Assign("Position", mesh.positions[i]);
        }
        if (mesh.HasNormals()) {
            arr.Assign("Normal", mesh.normals[i]);
        }
        if (mesh.HasTextureCoords()) {
            arr.Assign("TexCoord", mesh.textureCoords[i]);
        }
        arr.PushBack();
    }

    return arr;
}
