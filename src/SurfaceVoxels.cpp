#include "SurfaceVoxels.hpp"
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

std::vector<Voxel> const& SurfaceVoxels::Voxels()
{
    return m_voxels;
}

Mesh SurfaceVoxels::GenMesh()
{
    Mesh mesh;

    for (auto const& vx : m_voxels) {
        if (vx.vis & Voxel::Vis::XP) {
            AddFace(mesh, VOXEL.face.xp, vx.pos, m_scale);
        }
        if (vx.vis & Voxel::Vis::XN) {
            AddFace(mesh, VOXEL.face.xn, vx.pos, m_scale);
        }
        if (vx.vis & Voxel::Vis::YP) {
            AddFace(mesh, VOXEL.face.yp, vx.pos, m_scale);
        }
        if (vx.vis & Voxel::Vis::YN) {
            AddFace(mesh, VOXEL.face.yn, vx.pos, m_scale);
        }
        if (vx.vis & Voxel::Vis::ZP) {
            AddFace(mesh, VOXEL.face.zp, vx.pos, m_scale);
        }
        if (vx.vis & Voxel::Vis::ZN) {
            AddFace(mesh, VOXEL.face.zn, vx.pos, m_scale);
        }
    }

    mesh.textureCoords = std::vector<glm::vec2>(mesh.positions.size(), glm::vec2(0.0f, 0.0f));

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

void AddFace(Mesh& mesh, Mesh const& face, glm::vec3 offset, glm::vec3 scale)
{
    for (auto const& pos : face.positions) {
        mesh.positions.emplace_back(pos * scale + offset);
    }
    mesh.normals.insert(mesh.normals.end(), face.normals.begin(), face.normals.end());
}
