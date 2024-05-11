#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <vector>

#include "Vec.hpp"
#include "gfx/VertexArray.hpp"

class VertexBuffer;

struct BoundingBox {
    glm::vec3 max;
    glm::vec3 min;
};

using TriangularFace = Vec3<unsigned int>;

struct Mesh {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoords;
    std::vector<TriangularFace> faces;

    bool HasPositions() const;
    bool HasNormals() const;
    bool HasTextureCoords() const;
    bool HasFaces() const;
};

VertexArray GenVertexArray(Mesh const& mesh);

#endif
