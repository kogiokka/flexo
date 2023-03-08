#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <utility>

#include "TransformStack.hpp"
#include "VecUtil.hpp"
#include "VolumetricModelData.hpp"
#include "object/Map.hpp"
#include "object/SurfaceVoxels.hpp"
#include "util/Logger.h"

class Trianglemeter
{
public:
    Trianglemeter(glm::vec3 A, glm::vec3 B, glm::vec3 C);
    glm::vec3 NearestPoint(glm::vec3 point);
    glm::vec3 Barycentric(glm::vec3 point);

private:
    using Vertex = glm::vec3;
    class Edge
    {
    public:
        Edge(glm::vec3 const& tail, glm::vec3 const& head)
            : tail(tail)
            , head(head)
        {
        }

        glm::vec3 const& tail;
        glm::vec3 const& head;
    };

    glm::vec3 NearestOnEdge(glm::vec3 point, Trianglemeter::Edge const& edge);

    glm::vec3 m_normal;
    Vertex A;
    Vertex B;
    Vertex C;
    Edge BC;
    Edge CA;
    Edge AB;
};

using VoxelFaceList = std::array<EditableMesh, 6>;
static VoxelFaceList ConstructVoxelFaceList();
static void AddFace(EditableMesh& mesh, EditableMesh const& voxelFace, glm::vec3 position, glm::vec3 scale);

float SquaredDistance(glm::vec3 p1, glm::vec3 p2);

SurfaceVoxels::SurfaceVoxels(VolumetricModelData& modelData)
{
    glm::ivec3 n = modelData.GetResolution();
    m_scale = modelData.GetVoxelDims();

    const unsigned char* data = modelData.GetBuffer();

    const unsigned char modelValue = 255;
    const unsigned char air = 0;

    glm::vec3 origin = -0.5f * glm::vec3 { (n.x - 1), (n.y - 1), (0 - 1) };

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
                        m_voxels.emplace_back((origin + offset) * m_scale, glm::vec2(0.0f, 0.0f), vis);
                    }
                }
            }
        }
    }

    GenerateMesh();
}

std::vector<Voxel> const& SurfaceVoxels::Voxels() const
{
    return m_voxels;
}

void SurfaceVoxels::GenerateMesh()
{
    static VoxelFaceList faces = ConstructVoxelFaceList();

    EditableMesh mesh;

    for (auto const& vx : m_voxels) {
        if (vx.vis & VoxelVis_XPos) {
            AddFace(mesh, faces[0], vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), faces[0].positions.size(), vx.uv);
        }
        if (vx.vis & VoxelVis_XNeg) {
            AddFace(mesh, faces[1], vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), faces[1].positions.size(), vx.uv);
        }
        if (vx.vis & VoxelVis_YPos) {
            AddFace(mesh, faces[2], vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), faces[2].positions.size(), vx.uv);
        }
        if (vx.vis & VoxelVis_YNeg) {
            AddFace(mesh, faces[3], vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), faces[3].positions.size(), vx.uv);
        }
        if (vx.vis & VoxelVis_ZPos) {
            AddFace(mesh, faces[4], vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), faces[4].positions.size(), vx.uv);
        }
        if (vx.vis & VoxelVis_ZNeg) {
            AddFace(mesh, faces[5], vx.pos, m_scale);
            mesh.textureCoords.insert(mesh.textureCoords.end(), faces[5].positions.size(), vx.uv);
        }
    }

    m_mesh = mesh;
}

void SurfaceVoxels::ApplyTransform()
{
    auto mat = GenerateTransformStack().GenerateMatrix();
    m_scale *= m_transform.scale; // Voxel will have the same scaling factors as the model.
    for (auto& vx : m_voxels) {
        vx.pos = glm::vec3(mat * glm::vec4(vx.pos, 1.0f));
    }
    m_transform = Transform();
    GenerateMesh();
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

    auto const& mesh = map.GetMesh();
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

Trianglemeter::Trianglemeter(glm::vec3 A, glm::vec3 B, glm::vec3 C)
    : A(A)
    , B(B)
    , C(C)
    , BC(B, C)
    , CA(C, A)
    , AB(A, B)
{
    m_normal = glm::normalize(glm::cross(B - A, C - A));
}

glm::vec3 Trianglemeter::NearestPoint(glm::vec3 point)
{
    using glm::dot;

    glm::vec3 P = point + -m_normal * dot(point - A, m_normal);
    glm::vec3 weights = Barycentric(P);

    // BC, CA, AB
    if (weights.x < 0.0f) {
        return NearestOnEdge(P, BC);
    } else if (weights.y < 0.0f) {
        return NearestOnEdge(P, CA);
    } else if (weights.z < 0.0f) {
        return NearestOnEdge(P, AB);
    }

    return P;
}

glm::vec3 Trianglemeter::NearestOnEdge(glm::vec3 point, Trianglemeter::Edge const& edge)
{
    using glm::dot;

    glm::vec3 vec = edge.head - edge.tail;
    float len = dot(vec, vec);

    float portion = -1.0f;
    if (len != 0.0f) {
        portion = dot(point - edge.tail, vec) / len;
    }

    if (portion > 1.0f) {
        return edge.head;
    } else if (portion < 0.0f) {
        return edge.tail;
    } else {
        return edge.tail + portion * vec;
    }
}

glm::vec3 Trianglemeter::Barycentric(glm::vec3 point)
{
    using glm::dot, glm::cross;

    // Calculate the area ratios
    // The vertices are in counter-clockwise order.
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

VoxelFaceList ConstructVoxelFaceList()
{
    VoxelFaceList list;

    EditableMesh plane;
    TransformStack ts;

    // +X
    ts.PushTranslate(0.5f, 0.0f, 0.0f);
    ts.PushRotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    plane = ConstructPlane(1.0f);
    ts.Apply(plane.positions);
    list[0] = plane;

    // -X
    ts.Clear();
    ts.PushTranslate(-0.5f, 0.0f, 0.0f);
    ts.PushRotate(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    plane = ConstructPlane(1.0f);
    ts.Apply(plane.positions);
    list[1] = plane;

    // +Y
    ts.Clear();
    ts.PushTranslate(0.0f, 0.5f, 0.0f);
    ts.PushRotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    plane = ConstructPlane(1.0f);
    ts.Apply(plane.positions);
    list[2] = plane;

    // -Y
    ts.Clear();
    ts.PushTranslate(0.0f, -0.5f, 0.0f);
    ts.PushRotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    plane = ConstructPlane(1.0f);
    ts.Apply(plane.positions);
    list[3] = plane;

    // +Z
    ts.Clear();
    ts.PushTranslate(0.0f, 0.0f, 0.5f);
    plane = ConstructPlane(1.0f);
    ts.Apply(plane.positions);
    list[4] = plane;

    // -Z
    ts.Clear();
    ts.PushTranslate(0.0f, 0.0f, -0.5f);
    plane = ConstructPlane(1.0f);
    ts.Apply(plane.positions);
    list[5] = plane;

    return list;
}

void AddFace(EditableMesh& mesh, EditableMesh const& voxelFace, glm::vec3 position, glm::vec3 scale)
{
    unsigned int offset = mesh.positions.size();

    for (auto const& pos : voxelFace.positions) {
        mesh.positions.emplace_back(pos * scale + position);
    }

    for (auto face : voxelFace.faces) {
        for (auto& idx : face) {
            idx += offset;
        }
        mesh.faces.push_back(std::move(face));
    }
}
