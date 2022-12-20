#include <unordered_map>

#include <glm/gtc/matrix_transform.hpp>

#include "EditableMesh.hpp"
#include "assetlib/STL/STLImporter.hpp"

bool EditableMesh::HasPositions() const
{
    return !positions.empty();
}

bool EditableMesh::HasTextureCoords() const
{
    return !textureCoords.empty();
}

EditableMesh::TransformStack::TransformStack()
    : stack(glm::mat4(1.0f))
{
}

void EditableMesh::TransformStack::PushScale(glm::vec3 scale)
{
    stack = glm::scale(stack, scale);
}

void EditableMesh::TransformStack::PushTranslate(glm::vec3 translate)
{
    stack = glm::translate(stack, translate);
}

void EditableMesh::TransformStack::PushScale(float x, float y, float z)
{
    TransformStack::PushScale(glm::vec3(x, y, z));
}

void EditableMesh::TransformStack::PushTranslate(float x, float y, float z)
{
    TransformStack::PushTranslate(glm::vec3(x, y, z));
}

void EditableMesh::TransformStack::Apply(EditableMesh& mesh)
{
    for (auto& p : mesh.positions) {
        p = glm::vec3(stack * glm::vec4(p, 1.0f));
    }
}

EditableMesh ConstructCube()
{
    EditableMesh mesh;
    mesh.positions = {
        // 0
        { -1.0f, -1.0f, -1.0f },
        { 1.0f, -1.0f, -1.0f },
        { 1.0f, 1.0f, -1.0f },
        { -1.0f, 1.0f, -1.0f },
        // 4
        { -1.0f, -1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f },
    };

    mesh.faces = {
        // z
        { 0, 1, 2, 3 },
        // -z
        { 4, 5, 6, 7 },
        // -y
        { 4, 5, 1, 0 },
        // y
        { 6, 7, 3, 2 },
        // x
        { 1, 2, 6, 5 },
        // -x
        { 3, 0, 4, 7 },
    };
    return mesh;
}

EditableMesh ConstructSphere(int numSegments, int numRings)
{
    EditableMesh mesh;

    glm::vec3 center = { 0.0f, 0.0f, 0.0f };
    float radius = 1.0f;

    float deltaLong = glm::radians(360.0f) / numSegments;
    float deltaLat = glm::radians(180.0f) / numRings;

    mesh.positions.resize((numRings - 1) * numSegments + 2);
    mesh.positions.front() = center + radius * glm::vec3(0.0f, cos(0.0f), 0.0f);
    mesh.positions.back() = center + radius * glm::vec3(0.0f, cos(glm::radians(180.0f)), 0.0f);

    // Iterate through the rings without the first and the last.
    for (int i = 1; i < numRings; i++) {
        for (int j = 0; j < numSegments; j++) {
            float const theta = deltaLat * i;
            float const phi = glm::radians(180.0f) - deltaLong * j;
            glm::vec3 const coord = radius * glm::vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
            int const idx = (i - 1) * numSegments + j + 1;
            mesh.positions[idx] = center + coord;
        }
    }

    for (int j = 0; j < numSegments; j++) {
        unsigned int i1, i2, i3;
        i1 = 0;
        i2 = (j + 1) % numSegments + 1;
        i3 = j % numSegments + 1;
        mesh.faces.push_back({ i1, i2, i3 });
    }

    for (int i = 0; i < numRings - 2; i++) {
        for (int j = 0; j < numSegments; j++) {
            /**
             *  4---3
             *  |   |
             *  1---2
             */
            unsigned int i1, i2, i3, i4;
            i1 = (i * numSegments + (j % numSegments)) + 1;
            i2 = (i * numSegments + ((j + 1) % numSegments)) + 1;
            i3 = ((i + 1) * numSegments + ((j + 1) % numSegments)) + 1;
            i4 = ((i + 1) * numSegments + (j % numSegments)) + 1;
            mesh.faces.push_back({ i1, i2, i3, i4 });
        }
    }

    for (int j = 0; j < numSegments; j++) {
        unsigned int i1, i2, i3;
        i1 = ((numRings - 2) * numSegments + 1) + (j % numSegments);
        i2 = ((numRings - 2) * numSegments + 1) + ((j + 1) % numSegments);
        i3 = mesh.positions.size() - 1;
        mesh.faces.push_back({ i1, i2, i3 });
    }

    return mesh;
}

EditableMesh ConstructGrid(int numXDiv, int numYDiv)
{
    EditableMesh mesh;

    float ox = -1.0f;
    float oy = -1.0f;
    float dx = 2.0f / static_cast<float>(numXDiv);
    float dy = 2.0f / static_cast<float>(numYDiv);

    for (int y = 0; y < numYDiv; y++) {
        for (int x = 0; x < numXDiv; x++) {
            mesh.positions.emplace_back(ox + x * dx, oy + y * dy, 0.0f);
        }
    }

    for (int y = 0; y < numYDiv - 1; y++) {
        for (int x = 0; x < numXDiv - 1; x++) {
            unsigned int index = y * numXDiv + x;

            unsigned int i1, i2, i3, i4;
            i1 = index;
            i2 = index + 1;
            i3 = index + numXDiv + 1;
            i4 = index + numXDiv;
            mesh.faces.push_back({ i1, i2, i3, i4 });
        }
    }

    return mesh;
}

std::vector<TriangularFace> EditableMesh::GenerateTriangularFaces() const
{
    using VertexCount = unsigned int;
    using ListOfTriangles = std::vector<TriangularFace>;
    static std::unordered_map<VertexCount, ListOfTriangles> cache;

    ListOfTriangles triangles;

    for (Face const& face : faces) {
        VertexCount count = face.size();
        // Convex polygon triangulation
        if (cache.find(count) == cache.end()) {
            ListOfTriangles triplets;
            triplets.reserve(3 * (count - 2));
            for (unsigned int i = 1; i <= count - 2; ++i) {
                triplets.emplace_back(0, i, i + 1);
            }
            cache.insert({ count, triplets });
        }

        for (auto const& [x, y, z] : cache[count]) {
            triangles.emplace_back(face[x], face[y], face[z]);
        }
    }

    return triangles;
}

Mesh EditableMesh::GenerateMesh() const
{
    Mesh mesh;
    unsigned int size = faces.size() * 3;
    mesh.positions.reserve(size);
    mesh.normals.reserve(size);

    auto triangles = GenerateTriangularFaces();

    for (auto const& f : triangles) {
        glm::vec3 p1, p2, p3;
        glm::vec3 normal;

        p1 = positions[f.x];
        p2 = positions[f.y];
        p3 = positions[f.z];
        normal = glm::cross(p2 - p1, p3 - p1);

        mesh.positions.push_back(positions[f.x]);
        mesh.positions.push_back(positions[f.y]);
        mesh.positions.push_back(positions[f.z]);
        mesh.normals.push_back(normal);
        mesh.normals.push_back(normal);
        mesh.normals.push_back(normal);

        if (HasTextureCoords()) {
            mesh.textureCoords.push_back(textureCoords[f.x]);
            mesh.textureCoords.push_back(textureCoords[f.y]);
            mesh.textureCoords.push_back(textureCoords[f.z]);
        }
    }

    if (!HasTextureCoords()) {
        mesh.textureCoords = std::vector<glm::vec2>(mesh.positions.size(), glm::vec2(0.0f, 0.0f));
    }

    return mesh;
}

Wireframe EditableMesh::GenerateWireframe() const
{
    Wireframe wf;

    wf.positions = positions;
    /**
     *  3---2
     *  |   |
     *  0---1
     */
    for (auto const& face : faces) {
        unsigned int count = face.size();
        for (unsigned int i = 0; i < count; i++) {
            wf.edges.emplace_back(face[i], face[(i + 1) % count]);
        }
    }

    return wf;
}
