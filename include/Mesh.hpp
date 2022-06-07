#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <vector>

struct Face {
    std::vector<unsigned int> indices;
};

struct Mesh {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoords;
    std::vector<Face> faces;

    bool HasPositions() const;
    bool HasNormals() const;
    bool HasTextureCoords() const;
    bool HasFaces() const;
};

#endif
