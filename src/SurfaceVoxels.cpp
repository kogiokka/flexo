#include <algorithm>
#include <cmath>

#include "common/Logger.hpp"

#include "SurfaceVoxels.hpp"
#include "VecUtil.hpp"
#include "World.hpp"
#include "assetlib/STL/STLImporter.hpp"

void AddFace(Mesh& mesh, Mesh const& face, glm::vec3 offset, glm::vec3 scale);

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
    std::vector<Plane> planes;

    int const width = map.size.x;
    int const height = map.size.y;

    for (int y = 0; y < height - 1; ++y) {
        for (int x = 0; x < width - 1; ++x) {
            unsigned int const idx = y * width + x;

            Plane plane1, plane2;
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
            plane1.indices = { i1, i2, i3 };
            plane1.normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));

            plane2.indices = { i1, i3, i4 };
            plane2.normal = glm::normalize(glm::cross(p3 - p1, p4 - p1));

            planes.push_back(plane1);
            planes.push_back(plane2);
        }
    }

    for (auto& vx : m_voxels) {
        unsigned int index = 0;
        float minProj = 0.0f;
        float minDist = std::numeric_limits<float>::max();
        for (unsigned int i = 0; i < planes.size(); i++) {
            glm::vec3 a = VECCONV(map.nodes[planes[i].indices[0]].weights);
            float proj = glm::dot(vx.pos - a, planes[i].normal);
            float dist = std::abs(proj);
            if (dist < minDist) {
                index = i;
                minProj = proj;
                minDist = dist;
            }
        }

        Plane const& plane = planes[index];
        auto const& nodeA = map.nodes[plane.indices[0]];
        auto const& nodeB = map.nodes[plane.indices[1]];
        auto const& nodeC = map.nodes[plane.indices[2]];
        glm::vec3 A = VECCONV(nodeA.weights);
        glm::vec3 B = VECCONV(nodeB.weights);
        glm::vec3 C = VECCONV(nodeC.weights);

        glm::vec3 P = vx.pos + -plane.normal * minProj;

        glm::vec3 AP = A - P;
        glm::vec3 BP = B - P;
        glm::vec3 CP = C - P;

        float areaBCP = glm::length(glm::cross(BP, CP)) * 0.5f;
        float areaCAP = glm::length(glm::cross(CP, AP)) * 0.5f;
        float areaABP = glm::length(glm::cross(AP, BP)) * 0.5f;
        float area = glm::length(glm::cross(B - A, C - A)) * 0.5f;
        glm::vec3 weight = glm::vec3(areaBCP / area, areaCAP / area, areaABP / area);

        Logger::info("Areas : %.3f, %.3f, %.3f, All: %.3f", areaBCP, areaCAP, areaABP, area);
        // Logger::info("Barycentric Weights: %.3f, %.3f, %.3f", weight.x, weight.y, weight.z);
        Logger::info("Barycentric Weights Sum: %.6f", weight.x + weight.y + weight.z);
        vx.uv = VECCONV(nodeA.uv * weight.x) + VECCONV(nodeB.uv * weight.y) + VECCONV(nodeC.uv * weight.z);
    }
}

void AddFace(Mesh& mesh, Mesh const& face, glm::vec3 offset, glm::vec3 scale)
{
    for (auto const& pos : face.positions) {
        mesh.positions.emplace_back(pos * scale + offset);
    }
    mesh.normals.insert(mesh.normals.end(), face.normals.begin(), face.normals.end());
}

