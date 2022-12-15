#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include "SurfaceVoxels.hpp"
#include "VecUtil.hpp"
#include "World.hpp"
#include "assetlib/STL/STLImporter.hpp"

struct TrianglePlane {
    Vec3i vidx;
    glm::vec3 normal;
};

struct LineSeg {
    glm::vec3 v1, v2;
};

struct LineSegResult {
    float portion;
    bool success;
};

void AddFace(Mesh& mesh, Mesh const& face, glm::vec3 offset, glm::vec3 scale);

glm::vec3 FindPointOnTriangle(Map<3, 2> const& map, glm::vec3 point, TrianglePlane const& triangle);
LineSegResult FindPositionOnLineSeg(glm::vec3 point, LineSeg seg);

glm::vec3 Barycentric(glm::vec3 point, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
std::vector<TrianglePlane> MapFaces(Map<3, 2> const& map);

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
    std::vector<TrianglePlane> planes = MapFaces(map);

    for (auto& vx : m_voxels) {
        glm::vec3 closest;
        TrianglePlane target;
        float minDistSqr = std::numeric_limits<float>::max();

        for (auto const& plane : planes) {
            glm::vec3 point = FindPointOnTriangle(map, vx.pos, plane);
            glm::vec3 v = vx.pos - point;
            float distSqr = glm::dot(v, v);
            if (minDistSqr > distSqr) {
                minDistSqr = distSqr;
                closest = point;
                target = plane;
            }
        }

        auto weights
            = Barycentric(closest, VECCONV(map.nodes[target.vidx.x].weights), VECCONV(map.nodes[target.vidx.y].weights),
                          VECCONV(map.nodes[target.vidx.z].weights));
        vx.uv = VECCONV(map.nodes[target.vidx.x].uv) * weights.x + VECCONV(map.nodes[target.vidx.y].uv) * weights.y
            + VECCONV(map.nodes[target.vidx.z].uv) * weights.z;
    }
}

glm::vec3 FindPointOnTriangle(Map<3, 2> const& map, glm::vec3 point, TrianglePlane const& triangle)
{
    auto A = VECCONV(map.nodes[triangle.vidx.x].weights);
    auto B = VECCONV(map.nodes[triangle.vidx.y].weights);
    auto C = VECCONV(map.nodes[triangle.vidx.z].weights);
    auto normal = triangle.normal;

    // Test if the point is within the triangle
    {
        // Project the voxel position onto the plane
        glm::vec3 P = point + -normal * glm::dot(point - A, normal);

        glm::vec3 AP = A - P;
        glm::vec3 BP = B - P;
        glm::vec3 CP = C - P;
        bool res1 = glm::dot(glm::cross(AP, BP), normal) < 0.0f;
        bool res2 = glm::dot(glm::cross(BP, CP), normal) < 0.0f;
        bool res3 = glm::dot(glm::cross(CP, AP), normal) < 0.0f;

        if ((res1 == res2) && (res2 == res3)) {
            // the point is in the triangle
            return P;
        }
    }

    // Test if the position is on any of the edges
    {
        auto [p1, res1] = FindPositionOnLineSeg(point, { A, B });
        auto [p2, res2] = FindPositionOnLineSeg(point, { B, C });
        auto [p3, res3] = FindPositionOnLineSeg(point, { C, A });

        if (res1 || res2 || res3) {
            glm::vec3 pos(0.0f);
            float minDist = std::numeric_limits<float>::max();
            if (res1) {
                pos = A + p1 * (B - A);
                minDist = glm::length(point - pos);
            }
            if (res2) {
                auto tmp = B + p2 * (C - B);
                auto dist = glm::length(point - tmp);
                if (minDist > dist) {
                    pos = tmp;
                    minDist = dist;
                }
            }
            if (res3) {
                auto tmp = C + p3 * (A - C);
                auto dist = glm::length(point - tmp);
                if (minDist > dist) {
                    pos = tmp;
                    minDist = dist;
                }
            }

            return pos;
        }
    }

    // Find the closest vertex from the point
    {
        float d1 = glm::distance(point, A);
        float d2 = glm::distance(point, B);
        float d3 = glm::distance(point, C);

        glm::vec3 pos = A;
        float minDist = d1;

        if (minDist > d2) {
            minDist = d2;
            pos = B;
        }
        if (minDist > d3) {
            minDist = d3;
            pos = C;
        }

        return pos;
    }
}

LineSegResult FindPositionOnLineSeg(glm::vec3 point, LineSeg seg)
{
    auto const& [A, B] = seg;
    glm::vec3 AB = B - A;
    float portion = -1.0f;
    float lensqr = glm::dot(AB, AB);

    if (lensqr != 0.0f) {
        portion = glm::dot(point - A, AB) / lensqr;
    }

    return { portion, ((portion > 0.0f) && (portion < 1.0f)) };
}

glm::vec3 Barycentric(glm::vec3 point, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
    glm::vec3 AP = v1 - point;
    glm::vec3 BP = v2 - point;
    glm::vec3 CP = v3 - point;
    float BCP = glm::length(glm::cross(BP, CP));
    float CAP = glm::length(glm::cross(CP, AP));
    float ABP = glm::length(glm::cross(AP, BP));
    float total = glm::length(glm::cross(v2 - v1, v3 - v1));
    glm::vec3 weights = { BCP / total, CAP / total, ABP / total };
    return weights;
}

std::vector<TrianglePlane> MapFaces(const Map<3, 2>& map)
{
    std::vector<TrianglePlane> faces;

    int const width = map.size.x;
    int const height = map.size.y;

    for (int y = 0; y < height - 1; ++y) {
        for (int x = 0; x < width - 1; ++x) {
            unsigned int const idx = y * width + x;

            TrianglePlane plane1, plane2;
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
            plane1.vidx = { i1, i2, i3 };
            plane1.normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));

            plane2.vidx = { i1, i3, i4 };
            plane2.normal = glm::normalize(glm::cross(p3 - p1, p4 - p1));

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
