#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include "common/Logger.hpp"

#include "SurfaceVoxels.hpp"
#include "VecUtil.hpp"
#include "World.hpp"
#include "assetlib/STL/STLImporter.hpp"

struct TrianglePoint {
    enum Type {
        OnFace,
        OnEdge,
        OnVertex,
    };

    Vec3<bool> vidx;
    float distance;
    glm::vec3 position;
    Type type;
};

struct TrianglePlane {
    Vec3i vidx;
    glm::vec3 normal;
};

struct TargetTriangle {
    TrianglePlane plane;
    TrianglePoint point;
};

struct LineSeg {
    glm::vec3 v1, v2;
};

struct LineSegResult {
    float portion;
    bool success;
};

void AddFace(Mesh& mesh, Mesh const& face, glm::vec3 offset, glm::vec3 scale);

TrianglePoint FindPointOnTriangle(Map<3, 2> const& map, glm::vec3 point, TrianglePlane const& triangle);
LineSegResult FindPositionOnLineSeg(glm::vec3 point, LineSeg seg);

glm::vec3 Barycentric(Map<3, 2> const& map, TargetTriangle triangle);
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
        TargetTriangle target;
        float minDist = std::numeric_limits<float>::max();
        for (auto const& plane : planes) {
            TrianglePoint point = FindPointOnTriangle(map, vx.pos, plane);
            if (minDist > point.distance) {
                minDist = point.distance;
                target.point = point;
                target.plane = plane;
            }
        }
        auto weights = Barycentric(map, target);
        vx.uv = VECCONV(map.nodes[target.plane.vidx.x].uv) * weights.x
            + VECCONV(map.nodes[target.plane.vidx.y].uv) * weights.y
            + VECCONV(map.nodes[target.plane.vidx.z].uv) * weights.z;
    }
}

TrianglePoint FindPointOnTriangle(Map<3, 2> const& map, glm::vec3 point, TrianglePlane const& triangle)
{
    TrianglePoint tripos;

    auto A = VECCONV(map.nodes[triangle.vidx.x].weights);
    auto B = VECCONV(map.nodes[triangle.vidx.y].weights);
    auto C = VECCONV(map.nodes[triangle.vidx.z].weights);
    auto normal = triangle.normal;

    // Project the voxel position onto the plane
    glm::vec3 P = point + -normal * glm::dot(point - A, normal);

    glm::vec3 AP = A - P;
    glm::vec3 BP = B - P;
    glm::vec3 CP = C - P;

    // Test if the point is in the triangle
    {
        bool res1 = glm::dot(glm::cross(AP, BP), normal) < 0.0f;
        bool res2 = glm::dot(glm::cross(BP, CP), normal) < 0.0f;
        bool res3 = glm::dot(glm::cross(CP, AP), normal) < 0.0f;

        if ((res1 == res2) && (res2 == res3)) {
            // the point is in the triangle
            tripos.position = P;
            tripos.vidx = { true, true, true };
            tripos.type = TrianglePoint::OnFace;
            tripos.distance = glm::distance(point, P);
            return tripos;
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
            Vec3<bool> vidx;
            if (res1) {
                pos = A + p1 * (B - A);
                minDist = glm::length(point - pos);
                vidx = { true, true, false };
            }
            if (res2) {
                auto tmp = B + p2 * (C - B);
                auto dist = glm::length(point - tmp);
                if (minDist > dist) {
                    pos = tmp;
                    minDist = dist;
                    vidx = { false, true, true };
                }
            }
            if (res3) {
                auto tmp = C + p3 * (A - C);
                auto dist = glm::length(point - tmp);
                if (minDist > dist) {
                    pos = tmp;
                    minDist = dist;
                    vidx = { true, false, true };
                }
            }

            tripos.position = pos;
            tripos.vidx = vidx;
            tripos.type = TrianglePoint::OnEdge;
            tripos.distance = glm::distance(point, pos);
            return tripos;
        }
    }

    // Find the closest vertex from the point
    {
        float d1 = glm::distance(point, A);
        float d2 = glm::distance(point, B);
        float d3 = glm::distance(point, C);

        glm::vec3 pos = A;
        float minDist = d1;
        Vec3<bool> vidx = { true, false, false };

        if (minDist > d2) {
            minDist = d2;
            pos = B;
            vidx = { false, true, false };
        }
        if (minDist > d3) {
            minDist = d3;
            pos = C;
            vidx = { false, false, true };
        }

        tripos.position = pos;
        tripos.vidx = vidx;
        tripos.type = TrianglePoint::OnVertex;
        tripos.distance = glm::distance(point, pos);
        return tripos;
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

glm::vec3 Barycentric(Map<3, 2> const& map, TargetTriangle triangle)
{
    auto const& [plane, point] = triangle;
    auto A = VECCONV(map.nodes[plane.vidx.x].weights);
    auto B = VECCONV(map.nodes[plane.vidx.y].weights);
    auto C = VECCONV(map.nodes[plane.vidx.z].weights);

    switch (point.type) {
    case TrianglePoint::OnFace: {
        glm::vec3 AP = A - point.position;
        glm::vec3 BP = B - point.position;
        glm::vec3 CP = C - point.position;
        float areaBCP = glm::length(glm::cross(BP, CP)) * 0.5f;
        float areaCAP = glm::length(glm::cross(CP, AP)) * 0.5f;
        float areaABP = glm::length(glm::cross(AP, BP)) * 0.5f;
        float area = glm::length(glm::cross(B - A, C - A)) * 0.5f;
        glm::vec3 weights = { areaBCP / area, areaCAP / area, areaABP / area };
        return weights;
    }
    case TrianglePoint::OnEdge: {
        if (point.vidx.x && point.vidx.y) {
            auto [portion, res] = FindPositionOnLineSeg(point.position, { A, B });
            return { portion, 1.0f - portion, 0.0f };
        }
        if (point.vidx.y && point.vidx.z) {
            auto [portion, res] = FindPositionOnLineSeg(point.position, { B, C });
            return { portion, 1.0f - portion, 0.0f };
        }
        if (point.vidx.x && point.vidx.z) {
            auto [portion, res] = FindPositionOnLineSeg(point.position, { C, A });
            return { portion, 1.0f - portion, 0.0f };
        }
    }
    case TrianglePoint::OnVertex: {
        if (point.vidx.x) {
            return { 1.0f, 0.0f, 0.0f };
        }
        if (point.vidx.y) {
            return { 0.0f, 1.0f, 0.0f };
        }
        if (point.vidx.z) {
            return { 0.0f, 0.0f, 1.0f };
        }
    }
    }
    return { 0.0f, 0.0f, 0.0f };
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
