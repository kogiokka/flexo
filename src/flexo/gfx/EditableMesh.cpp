#include <unordered_map>

#include <glm/gtc/matrix_transform.hpp>

#include "gfx/EditableMesh.hpp"

bool EditableMesh::HasPositions() const
{
    return !positions.empty();
}

bool EditableMesh::HasTextureCoords() const
{
    return !textureCoords.empty();
}

EditableMesh ConstructPlane(float size)
{
    EditableMesh mesh;
    mesh.positions = {
        // 0
        0.5f * size * glm::vec3(-1.0f, -1.0f, 0.0f),
        0.5f * size * glm::vec3(+1.0f, -1.0f, 0.0f),
        0.5f * size * glm::vec3(+1.0f, +1.0f, 0.0f),
        0.5f * size * glm::vec3(-1.0f, +1.0f, 0.0f),
    };

    mesh.faces = {
        // z
        { 0, 1, 2, 3 },
    };

    mesh.textureCoords = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

    return mesh;
}

EditableMesh ConstructCube(float size)
{
    EditableMesh mesh;
    mesh.positions = {
        // 0
        0.5f * size * glm::vec3(-1.0f, -1.0f, -1.0f),
        0.5f * size * glm::vec3(+1.0f, -1.0f, -1.0f),
        0.5f * size * glm::vec3(+1.0f, +1.0f, -1.0f),
        0.5f * size * glm::vec3(-1.0f, +1.0f, -1.0f),
        // 4
        0.5f * size * glm::vec3(-1.0f, -1.0f, +1.0f),
        0.5f * size * glm::vec3(+1.0f, -1.0f, +1.0f),
        0.5f * size * glm::vec3(+1.0f, +1.0f, +1.0f),
        0.5f * size * glm::vec3(-1.0f, +1.0f, +1.0f),
    };

    mesh.faces = {
        // -z
        { 3, 2, 1, 0 },
        // z
        { 4, 5, 6, 7 },
        // -y
        { 0, 1, 5, 4 },
        // y
        { 2, 3, 7, 6 },
        // x
        { 1, 2, 6, 5 },
        // -x
        { 3, 0, 4, 7 },
    };

    return mesh;
}

EditableMesh ConstructUVSphere(int numSegments, int numRings, float radius)
{
    EditableMesh mesh;

    glm::vec3 center = { 0.0f, 0.0f, 0.0f };

    float deltaLong = glm::radians(360.0f) / numSegments;
    float deltaLat = glm::radians(180.0f) / numRings;

    mesh.positions.resize((numRings - 1) * numSegments + 2);

    // The first ring (top)
    mesh.positions.front() = center + glm::vec3(0.0f, 0.0f, radius);
    for (int j = 0; j < numSegments; j++) {
        unsigned int i1, i2, i3;
        i1 = 0; // zenith
        i2 = (j % numSegments) + 1;
        i3 = (j + 1) % numSegments + 1;
        mesh.faces.push_back({ i1, i2, i3 });
    }

    // The last ring (bottom)
    mesh.positions.back() = center + glm::vec3(0.0f, 0.0f, -radius);
    for (int j = 0; j < numSegments; j++) {
        unsigned int last = mesh.positions.size() - 1;
        unsigned int i1, i2, i3;
        i1 = (last - numSegments) + ((j + 1) % numSegments);
        i2 = (last - numSegments) + (j % numSegments);
        i3 = last;
        mesh.faces.push_back({ i1, i2, i3 });
    }

    // Vertices for rings without the first one
    for (int i = 0; i < numRings - 1; i++) {
        for (int j = 0; j < numSegments; j++) {
            float const theta = deltaLat * (i + 1);
            float const phi = deltaLong * j;
            glm::vec3 const coord = radius * glm::vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            unsigned int idx = (1 /* zenith vertex */) + (i * numSegments + j);
            mesh.positions[idx] = center + coord;
        }
    }

    // Indices for rings without the first one and the last one
    for (int i = 0; i < numRings - 2; i++) {
        for (int j = 0; j < numSegments; j++) {
            /**
             * Quad face on the sphere
             *
             *  1---4 ring i
             *  |   |
             *  2---3 ring i+1
             */
            unsigned int i1, i2, i3, i4;
            i1 = 1 + (i * numSegments + (j % numSegments));
            i2 = 1 + ((i + 1) * numSegments + (j % numSegments));
            i3 = 1 + ((i + 1) * numSegments + ((j + 1) % numSegments));
            i4 = 1 + (i * numSegments + ((j + 1) % numSegments));
            mesh.faces.push_back({ i1, i2, i3, i4 });
        }
    }

    return mesh;
}

EditableMesh ConstructTorus(int majorSeg, int minorSeg, float majorRad, float minorRad)
{
    EditableMesh mesh;

    using std::cos, std::sin;
    static float const pi = glm::radians(180.0f);

    glm::vec3 center = { 0.0f, 0.0f, 0.0f };
    float mjD = pi * 2.0f / majorSeg;
    float mnD = pi * 2.0f / minorSeg;
    for (int i = 0; i < majorSeg; i++) {
        float phi = i * mjD;
        float sP = sin(phi);
        float cP = cos(phi);
        glm::vec3 mnC = center + majorRad * glm::vec3(cP, sP, center.z);
        for (int j = 0; j < minorSeg; j++) {
            float theta = j * mnD;
            float sT = sin(theta);
            float cT = cos(theta);
            mesh.positions.emplace_back(mnC + minorRad * glm::vec3(cT * cP, cT * sP, sT));
        }
    }

    for (int i = 0; i < majorSeg; i++) {
        for (int j = 0; j < minorSeg; j++) {
            for (int k = 0; k < 2; k++) {
                unsigned int i1, i2, i3, i4;
                i1 = (i % majorSeg) * minorSeg + (j + k) % minorSeg;
                i2 = ((i + 1) % majorSeg) * minorSeg + (j + k) % minorSeg;
                i3 = ((i + 1) % majorSeg) * minorSeg + (j + 1 + k) % minorSeg;
                i4 = (i % majorSeg) * minorSeg + (j + 1 + k) % minorSeg;
                mesh.faces.push_back({ i1, i2, i3, i4 });
            }
        }
    }

    return mesh;
}

EditableMesh ConstructGrid(int numXDiv, int numYDiv, float size)
{
    EditableMesh mesh;

    float ox = 0.5f * -size;
    float oy = 0.5f * -size;
    float dx = size / static_cast<float>(numXDiv);
    float dy = size / static_cast<float>(numYDiv);

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

EditableMesh ConstructIntervaledGrid(int numCell, int numBlock, float size)
{
    EditableMesh mesh;
    float blkSize = numCell / numBlock;
    float const cellSize = size / numCell;

    unsigned int begin = 0;
    unsigned int index = 0;
    for (int i = 0; i < numBlock; i++) {
        for (int j = 0; j < blkSize - 1; j++) {
            // X
            mesh.positions.emplace_back(+1.0f, -1.0f + (j + 1 + begin) * cellSize, 0.0f);
            mesh.positions.emplace_back(-1.0f, -1.0f + (j + 1 + begin) * cellSize, 0.0f);
            // Y
            mesh.positions.emplace_back(-1.0f + (j + 1 + begin) * cellSize, +1.0f, 0.0f);
            mesh.positions.emplace_back(-1.0f + (j + 1 + begin) * cellSize, -1.0f, 0.0f);

            mesh.faces.push_back({ index + 0, index + 1 });
            mesh.faces.push_back({ index + 2, index + 3 });
            index += 4;
        }
        begin += blkSize;
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
