#include "Mesh.hpp"

#include <cstddef>
#include <unordered_map>

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
