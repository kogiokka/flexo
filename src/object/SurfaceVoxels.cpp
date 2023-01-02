#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <utility>

#include "TransformStack.hpp"
#include "VecUtil.hpp"
#include "VolumetricModelData.hpp"
#include "World.hpp"
#include "object/Map.hpp"
#include "object/SurfaceVoxels.hpp"
#include "util/Logger.h"

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

void AddFace(Mesh& mesh, Mesh const& face, glm::vec3 offset, glm::vec3 scale);
float SquaredDistance(glm::vec3 p1, glm::vec3 p2);
void ConstructVoxelMesh();

std::array<Mesh, 6> VOXEL_MESH;

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
                VoxelVis vis = VoxelVis_None;
                glm::vec3 offset { k, j, i };
                if (data[index] == modelValue) {
                    if ((k + 1 == n.x) || data[index + 1] == air) {
                        vis |= VoxelVis_XPos;
                    }
                    if ((j + 1 == n.y) || (data[index + n.x] == air)) {
                        vis |= VoxelVis_YPos;
                    }
                    if ((i + 1 == n.z) || (data[index + n.x * n.y] == air)) {
                        vis |= VoxelVis_ZPos;
                    }
                    if ((k == 0) || (data[index - 1] == air)) {
                        vis |= VoxelVis_XNeg;
                    }
                    if ((j == 0) || (data[index - n.x] == air)) {
                        vis |= VoxelVis_YNeg;
                    }
                    if ((i == 0) || (data[index - n.x * n.y] == air)) {
                        vis |= VoxelVis_ZNeg;
                    }

                    if (vis != VoxelVis_None) {
                        m_voxels.emplace_back(offset * m_scale, glm::vec2(0.0f, 0.0f), vis);
                    }
                }
            }
        }
    }

    ConstructVoxelMesh();
}

std::vector<Voxel> const& SurfaceVoxels::Voxels() const
{
    return m_voxels;
}

void SurfaceVoxels::ApplyTransform()
{
    auto mat = GenerateTransformStack().GenerateMatrix();
    m_scale *= m_transform.scale; // Voxel will have the same scaling factors as the model.
    for (auto& vx : m_voxels) {
        vx.pos = glm::vec3(mat * glm::vec4(vx.pos, 1.0f));
    }
    m_transform = Transform();
}

Mesh SurfaceVoxels::GenerateMesh() const
{
    Mesh mesh;

    for (auto const& vx : m_voxels) {
        if (vx.vis & VoxelVis_XPos) {
            AddFace(mesh, VOXEL_MESH[0], vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), 6, vx.uv);
        }
        if (vx.vis & VoxelVis_XNeg) {
            AddFace(mesh, VOXEL_MESH[1], vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), 6, vx.uv);
        }
        if (vx.vis & VoxelVis_YPos) {
            AddFace(mesh, VOXEL_MESH[2], vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), 6, vx.uv);
        }
        if (vx.vis & VoxelVis_YNeg) {
            AddFace(mesh, VOXEL_MESH[3], vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), 6, vx.uv);
        }
        if (vx.vis & VoxelVis_ZPos) {
            AddFace(mesh, VOXEL_MESH[4], vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), 6, vx.uv);
        }
        if (vx.vis & VoxelVis_ZNeg) {
            AddFace(mesh, VOXEL_MESH[5], vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), 6, vx.uv);
        }
    }

    return mesh;
}

Wireframe SurfaceVoxels::GenerateWireMesh() const
{
    Wireframe wire;

    // TODO;
    return wire;
}

std::vector<glm::vec3> SurfaceVoxels::GetPositions() const
{
    std::vector<glm::vec3> pos(m_voxels.size());

    for (unsigned int i = 0; i < m_voxels.size(); i++) {
        pos[i] = m_voxels[i].pos;
    }

    return pos;
}

std::future<void> SurfaceVoxels::Parameterize(Map<3, 2> const& map, float& progress)
{
    progress = 0.0f;

    auto mesh = GenMapEditableMesh(map);
    auto const& faces = mesh.GenerateTriangularFaces();

    return std::async(
        std::launch::async,
        [this, &progress](EditableMesh const& mesh, std::vector<TriangularFace> const& faces) -> void {
            float const diff = 100.0f / static_cast<float>(m_voxels.size());
#pragma omp parallel for
            for (auto& vx : m_voxels) {
                glm::vec3 nearest;
                TriangularFace target;
                float minDist = std::numeric_limits<float>::max();

#pragma omp parallel for reduction(+ : progress)
                for (auto const& f : faces) {
                    auto meter = Trianglemeter(mesh.positions[f.x], mesh.positions[f.y], mesh.positions[f.z]);

                    glm::vec3 point = meter.NearestPoint(vx.pos);
                    float dist = SquaredDistance(vx.pos, point);
                    if (minDist > dist) {
                        minDist = dist;
                        nearest = point;
                        target = f;
                    }
                }

                auto weights
                    = Trianglemeter(mesh.positions[target.x], mesh.positions[target.y], mesh.positions[target.z])
                          .Barycentric(nearest);
                vx.uv = mesh.textureCoords[target.x] * weights.x + mesh.textureCoords[target.y] * weights.y
                    + mesh.textureCoords[target.z] * weights.z;

                progress += diff;
            }
        },
        mesh, faces);
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
    float minDist = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; i++) {
        glm::vec3 tmp;
        std::tie(tmp, success) = NearestOnEdge(point, m_edges[i]);
        if (success) {
            float dist = SquaredDistance(point, tmp);
            if (minDist > dist) {
                minDist = dist;
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

    glm::vec3 v1 = m_verts[edge.x];
    glm::vec3 v2 = m_verts[edge.y];

    float portion = -1.0f;
    float len = SquaredDistance(v1, v2);
    glm::vec3 vec = v2 - v1;

    if (len != 0.0f) {
        portion = glm::dot(point - v1, vec) / len;
        nearest = v1 + portion * vec;
    }

    return { nearest, ((portion >= 0.0f) && (portion <= 1.0f)) };
}

glm::vec3 Trianglemeter::NearestVertex(glm::vec3 point)
{
    glm::vec3 nearest;

    float minDist = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; i++) {
        float dist = SquaredDistance(point, m_verts[i]);
        if (minDist > dist) {
            minDist = dist;
            nearest = m_verts[i];
        }
    }

    return nearest;
}

glm::vec3 Trianglemeter::Barycentric(glm::vec3 point)
{
    using glm::dot, glm::cross;

    // The vertices are in counter-clockwise order.
    auto const& [A, B, C] = m_verts;

    // Calculate the area ratios
    float oppA = dot(m_normal, cross(B - point, C - point));
    float oppB = dot(m_normal, cross(C - point, A - point));
    float total = dot(m_normal, cross(B - A, C - A));
    float u = oppA / total;
    float v = oppB / total;
    glm::vec3 weights = { u, v, 1 - u - v };
    return weights;
}

float SquaredDistance(glm::vec3 p1, glm::vec3 p2)
{
    glm::vec3 v = p1 - p2;
    return glm::dot(v, v);
}

void ConstructVoxelMesh()
{
    EditableMesh plane;
    TransformStack ts;

    // +X
    ts.PushTranslate(1.0f, 0.0f, 0.0f);
    ts.PushRotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    plane = ConstructPlane();
    ts.Apply(plane.positions);
    VOXEL_MESH[0] = plane.GenerateMesh();

    // -X
    ts.Clear();
    ts.PushTranslate(-1.0f, 0.0f, 0.0f);
    ts.PushRotate(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    plane = ConstructPlane();
    ts.Apply(plane.positions);
    VOXEL_MESH[1] = plane.GenerateMesh();

    // +Y
    ts.Clear();
    ts.PushTranslate(0.0f, 1.0f, 0.0f);
    ts.PushRotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    plane = ConstructPlane();
    ts.Apply(plane.positions);
    VOXEL_MESH[2] = plane.GenerateMesh();

    // -Y
    ts.Clear();
    ts.PushTranslate(0.0f, -1.0f, 0.0f);
    ts.PushRotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    plane = ConstructPlane();
    ts.Apply(plane.positions);
    VOXEL_MESH[3] = plane.GenerateMesh();

    // +Z
    ts.Clear();
    ts.PushTranslate(0.0f, 0.0f, 1.0f);
    plane = ConstructPlane();
    ts.Apply(plane.positions);
    VOXEL_MESH[4] = plane.GenerateMesh();

    // -Z
    ts.Clear();
    ts.PushTranslate(0.0f, 0.0f, -1.0f);
    plane = ConstructPlane();
    ts.Apply(plane.positions);
    VOXEL_MESH[5] = plane.GenerateMesh();
}

void AddFace(Mesh& mesh, Mesh const& face, glm::vec3 offset, glm::vec3 scale)
{
    for (auto const& pos : face.positions) {
        mesh.positions.emplace_back(pos * scale + offset);
    }
    mesh.normals.insert(mesh.normals.end(), face.normals.begin(), face.normals.end());
}
