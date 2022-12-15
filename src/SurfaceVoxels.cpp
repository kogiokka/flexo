#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include "SurfaceVoxels.hpp"
#include "VecUtil.hpp"
#include "World.hpp"
#include "assetlib/STL/STLImporter.hpp"

class Trianglemeter
{
public:
    using Edge = Vec2i;
    using Result = std::pair<glm::vec3, bool>;

    Trianglemeter(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
    glm::vec3 NearestPoint(glm::vec3 point);
    glm::vec3 Barycentric(glm::vec3 point);

private:
    Result NearestOnFace(glm::vec3 point);
    Result NearestOnEdge(glm::vec3 point, Trianglemeter::Edge edge);
    glm::vec3 NearestVertex(glm::vec3 point);

    Vec3<glm::vec3> m_verts;
    Vec3<Edge> m_edges;

public:
    glm::vec3 m_normal;
};

using TriangleRef = Vec3i;

void AddFace(Mesh& mesh, Mesh const& face, glm::vec3 offset, glm::vec3 scale);

std::vector<TriangleRef> MapFaces(Map<3, 2> const& map);

static STLImporter STLI;

struct Cube {
    struct {
        Mesh xp;
        Mesh xn;
        Mesh yp;
        Mesh yn;
        Mesh zp;
        Mesh zn;
    } face;
};

static Cube const VOXEL = { {
    STLI.ReadFile("res/models/voxel/cube-x-pos.stl"),
    STLI.ReadFile("res/models/voxel/cube-x-neg.stl"),
    STLI.ReadFile("res/models/voxel/cube-y-pos.stl"),
    STLI.ReadFile("res/models/voxel/cube-y-neg.stl"),
    STLI.ReadFile("res/models/voxel/cube-z-pos.stl"),
    STLI.ReadFile("res/models/voxel/cube-z-neg.stl"),
} };

SurfaceVoxels::SurfaceVoxels(VolumetricModelData& modelData)
{
    glm::ivec3 n = modelData.GetResolution();
    m_scale = modelData.GetVoxelDims();

    const unsigned char* data = modelData.GetBuffer();

    const unsigned char modelValue = 255;
    const unsigned char air = 0;

    for (int i = 0; i < n.z; i++) {
        for (int j = 0; j < n.y; j++) {
            for (int k = 0; k < n.x; k++) {
                int index = k + j * n.x + i * n.x * n.y;
                unsigned char vis = Voxel::Vis::None;
                glm::vec3 offset { k, j, i };
                if (data[index] == modelValue) {
                    if ((k + 1 == n.x) || data[index + 1] == air) {
                        vis |= Voxel::Vis::XP;
                    }
                    if ((j + 1 == n.y) || (data[index + n.x] == air)) {
                        vis |= Voxel::Vis::YP;
                    }
                    if ((i + 1 == n.z) || (data[index + n.x * n.y] == air)) {
                        vis |= Voxel::Vis::ZP;
                    }
                    if ((k == 0) || (data[index - 1] == air)) {
                        vis |= Voxel::Vis::XN;
                    }
                    if ((j == 0) || (data[index - n.x] == air)) {
                        vis |= Voxel::Vis::YN;
                    }
                    if ((i == 0) || (data[index - n.x * n.y] == air)) {
                        vis |= Voxel::Vis::ZN;
                    }

                    if (vis != Voxel::Vis::None) {
                        m_voxels.emplace_back(offset * m_scale, glm::vec2(0.0f, 0.0f), static_cast<Voxel::Vis>(vis));
                    }
                }
            }
        }
    }
}

std::vector<Voxel> const& SurfaceVoxels::Voxels() const
{
    return m_voxels;
}

Mesh SurfaceVoxels::GenMesh()
{
    Mesh mesh;

    for (auto const& vx : m_voxels) {
        if (vx.vis & Voxel::Vis::XP) {
            AddFace(mesh, VOXEL.face.xp, vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), 6, vx.uv);
        }
        if (vx.vis & Voxel::Vis::XN) {
            AddFace(mesh, VOXEL.face.xn, vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), 6, vx.uv);
        }
        if (vx.vis & Voxel::Vis::YP) {
            AddFace(mesh, VOXEL.face.yp, vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), 6, vx.uv);
        }
        if (vx.vis & Voxel::Vis::YN) {
            AddFace(mesh, VOXEL.face.yn, vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), 6, vx.uv);
        }
        if (vx.vis & Voxel::Vis::ZP) {
            AddFace(mesh, VOXEL.face.zp, vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), 6, vx.uv);
        }
        if (vx.vis & Voxel::Vis::ZN) {
            AddFace(mesh, VOXEL.face.zn, vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), 6, vx.uv);
        }
    }

    return mesh;
}

std::vector<glm::vec3> SurfaceVoxels::GenPositions()
{
    std::vector<glm::vec3> pos(m_voxels.size());

    for (unsigned int i = 0; i < m_voxels.size(); i++) {
        pos[i] = m_voxels[i].pos;
    }

    return pos;
}

void SurfaceVoxels::Parameterize(Map<3, 2> const& map)
{
    std::vector<TriangleRef> planes = MapFaces(map);

    for (auto& vx : m_voxels) {
        glm::vec3 closest;
        TriangleRef target;
        float minDistSqr = std::numeric_limits<float>::max();

        for (auto const& plane : planes) {
            auto meter = Trianglemeter(VECCONV(map.nodes[plane.x].weights), VECCONV(map.nodes[plane.y].weights),
                                       VECCONV(map.nodes[plane.z].weights));

            glm::vec3 point = meter.NearestPoint(vx.pos);
            glm::vec3 v = vx.pos - point;
            float distSqr = glm::dot(v, v);
            if (minDistSqr > distSqr) {
                minDistSqr = distSqr;
                closest = point;
                target = plane;
            }
        }

        auto weights = Trianglemeter(VECCONV(map.nodes[target.x].weights), VECCONV(map.nodes[target.y].weights),
                                     VECCONV(map.nodes[target.z].weights))
                           .Barycentric(closest);
        vx.uv = VECCONV(map.nodes[target.x].uv) * weights.x + VECCONV(map.nodes[target.y].uv) * weights.y
            + VECCONV(map.nodes[target.z].uv) * weights.z;
    }
}

Trianglemeter::Trianglemeter(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
    : m_edges { Vec2i(0, 1), Vec2i(1, 2), Vec2i(2, 0) }
{
    m_verts = { v1, v2, v3 };
    m_normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
}

glm::vec3 Trianglemeter::NearestPoint(glm::vec3 point)
{
    glm::vec3 nearest;
    bool success = false;

    std::tie(nearest, success) = NearestOnFace(point);

    if (success) {
        return nearest;
    }

    // Test if the position is on any of the edges
    float minDistSqr = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; i++) {
        glm::vec3 tmp;
        std::tie(tmp, success) = NearestOnEdge(point, m_edges[i]);
        if (success) {
            glm::vec3 v = point - tmp;
            float distSqr = glm::dot(v, v);
            if (minDistSqr > distSqr) {
                minDistSqr = distSqr;
                nearest = tmp;
            }
        }
    }

    if (success) {
        return nearest;
    }

    return NearestVertex(point);
}

Trianglemeter::Result Trianglemeter::NearestOnFace(glm::vec3 point)
{
    auto const& [A, B, C] = m_verts;

    glm::vec3 P = point + -m_normal * glm::dot(point - A, m_normal);

    glm::vec3 AP = A - P;
    glm::vec3 BP = B - P;
    glm::vec3 CP = C - P;
    bool res1 = glm::dot(glm::cross(AP, BP), m_normal) < 0.0f;
    bool res2 = glm::dot(glm::cross(BP, CP), m_normal) < 0.0f;
    bool res3 = glm::dot(glm::cross(CP, AP), m_normal) < 0.0f;

    return { P, (res1 == res2) && (res2 == res3) };
}

Trianglemeter::Result Trianglemeter::NearestOnEdge(glm::vec3 point, Trianglemeter::Edge edge)
{
    glm::vec3 nearest;

    glm::vec3 A = m_verts[edge.x];
    glm::vec3 B = m_verts[edge.y];
    glm::vec3 AB = B - A;
    float portion = -1.0f;
    float lensqr = glm::dot(AB, AB);

    if (lensqr != 0.0f) {
        portion = glm::dot(point - A, AB) / lensqr;
        nearest = A + portion * (B - A);
    }

    return { nearest, ((portion >= 0.0f) && (portion <= 1.0f)) };
}

glm::vec3 Trianglemeter::NearestVertex(glm::vec3 point)
{
    glm::vec3 nearest;

    float minDistSqr = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; i++) {
        glm::vec3 v = point - m_verts[i];
        float distSqr = glm::dot(v, v);
        if (minDistSqr > distSqr) {
            minDistSqr = distSqr;
            nearest = m_verts[i];
        }
    }

    return nearest;
}

glm::vec3 Trianglemeter::Barycentric(glm::vec3 point)
{
    auto const& [A, B, C] = m_verts;
    glm::vec3 PA = A - point;
    glm::vec3 PB = B - point;
    glm::vec3 PC = C - point;
    float BCP = glm::length(glm::cross(PB, PC));
    float CAP = glm::length(glm::cross(PC, PA));
    float ABP = glm::length(glm::cross(PA, PB));
    float total = glm::length(glm::cross(B - A, C - A));
    glm::vec3 weights = { BCP / total, CAP / total, ABP / total };
    return weights;
}

std::vector<TriangleRef> MapFaces(const Map<3, 2>& map)
{
    std::vector<TriangleRef> faces;

    int const width = map.size.x;
    int const height = map.size.y;

    for (int y = 0; y < height - 1; ++y) {
        for (int x = 0; x < width - 1; ++x) {
            unsigned int const idx = y * width + x;

            TriangleRef plane1, plane2;
            int i1, i2, i3, i4;
            glm::vec3 p1, p2, p3, p4;

            /**
             *  4-----3
             *  |     |
             *  |     |
             *  1-----2
             */
            i1 = idx;
            i2 = idx + 1;
            i3 = idx + width + 1;
            i4 = idx + width;
            p1 = VECCONV(map.nodes[i1].weights); // [x, y]
            p2 = VECCONV(map.nodes[i2].weights); // [x + 1, y]
            p3 = VECCONV(map.nodes[i3].weights); // [x + 1, y + 1]
            p4 = VECCONV(map.nodes[i4].weights); // [x, y + 1]

            // Normals
            plane1 = { i1, i2, i3 };
            // plane1.normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));

            plane2 = { i1, i3, i4 };
            // plane2.normal = glm::normalize(glm::cross(p3 - p1, p4 - p1);

            faces.push_back(plane1);
            faces.push_back(plane2);
        }
    }

    return faces;
}

void AddFace(Mesh& mesh, Mesh const& face, glm::vec3 offset, glm::vec3 scale)
{
    for (auto const& pos : face.positions) {
        mesh.positions.emplace_back(pos * scale + offset);
    }
    mesh.normals.insert(mesh.normals.end(), face.normals.begin(), face.normals.end());
}
