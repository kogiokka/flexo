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

EditableMesh ConstructPlane();
EditableMesh ConstructCube();
EditableMesh ConstructSphere(int numSegments, int numRings);
EditableMesh ConstructTorus(int majorSeg, int minorSeg, float majorRad, float minorRad);
EditableMesh ConstructGrid(int numXDiv = 10, int numYDiv = 10);

#endif
