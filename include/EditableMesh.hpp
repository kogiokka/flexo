#ifndef EDITABLE_MESH_H
#define EDITABLE_MESH_H

#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "Vec.hpp"
#include "Wireframe.hpp"

using Face = std::vector<unsigned int>;
struct EditableMesh {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> textureCoords;
    std::vector<Face> faces;

    bool HasPositions() const;
    bool HasTextureCoords() const;
    std::vector<TriangularFace> GenerateTriangularFaces() const;
    Mesh GenerateMesh() const;
    Wireframe GenerateWireframe() const;
};

EditableMesh ConstructPlane(float size = 2.0f);
EditableMesh ConstructCube(float size = 2.0f);
EditableMesh ConstructSphere(int numSegments = 32, int numRings = 16, float radius = 1.0f);
EditableMesh ConstructTorus(int majorSeg = 48, int minorSeg = 12, float majorRad = 1.0f, float minorRad = 0.25f);
EditableMesh ConstructGrid(int numXDiv = 10, int numYDiv = 10);

#endif
