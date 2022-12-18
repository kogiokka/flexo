#ifndef EDITABLE_MESH_H
#define EDITABLE_MESH_H

#include <glm/glm.hpp>

#include "Mesh.hpp"

struct EditableMesh {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> textureCoords;
    std::vector<Face> faces;

    bool HasPositions() const;
    bool HasTextureCoords() const;
    Mesh GenerateMesh();

    struct TransformStack {
        TransformStack();
        void PushScale(glm::vec3 scale);
        void PushTranslate(glm::vec3 translate);
        void PushScale(float x, float y, float z);
        void PushTranslate(float x, float y, float z);
        void Apply(EditableMesh& mesh);

    private:
        glm::mat4 stack;
    };
};

EditableMesh ConstructCube();
EditableMesh ConstructSphere(int numSegments, int numRings);

#endif
