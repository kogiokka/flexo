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
    glm::vec3 dims = modelData.GetVoxelDims();
    const unsigned char* data = modelData.GetBuffer();

    const unsigned char modelValue = 255;
    const unsigned char air = 0;

    Mesh vxMesh;
    for (int i = 0; i < n.z; i++) {
        for (int j = 0; j < n.y; j++) {
            for (int k = 0; k < n.x; k++) {
                int index = k + j * n.x + i * n.x * n.y;
                glm::vec3 offset { k, j, i };
                if (data[index] == modelValue) {
                    if ((k + 1 == n.x) || data[index + 1] == air) {
                        AddFace(vxMesh, VOXEL.face.xp, offset, dims);
                    }
                    if ((j + 1 == n.y) || (data[index + n.x] == air)) {
                        AddFace(vxMesh, VOXEL.face.yp, offset, dims);
                    }
                    if ((i + 1 == n.z) || (data[index + n.x * n.y] == air)) {
                        AddFace(vxMesh, VOXEL.face.zp, offset, dims);
                    }
                    if ((k == 0) || (data[index - 1] == air)) {
                        AddFace(vxMesh, VOXEL.face.xn, offset, dims);
                    }
                    if ((j == 0) || (data[index - n.x] == air)) {
                        AddFace(vxMesh, VOXEL.face.yn, offset, dims);
                    }
                    if ((i == 0) || (data[index - n.x * n.y] == air)) {
                        AddFace(vxMesh, VOXEL.face.zn, offset, dims);
                    }
                }
            }
        }
    }

    m_voxels.emplace_back(vxMesh, glm::vec2(0.0f, 0.0f));
}

Mesh SurfaceVoxels::GenMesh()
{
    Mesh mesh;

    for (auto const& vx : m_voxels) {
        mesh.positions.insert(mesh.positions.end(), vx.mMesh.positions.begin(), vx.mMesh.positions.end());
        mesh.normals.insert(mesh.normals.end(), vx.mMesh.normals.begin(), vx.mMesh.normals.end());
        mesh.textureCoords = std::vector<glm::vec2>(mesh.positions.size(), glm::vec2(0.0f, 0.0f));
    }

    return mesh;
}

void AddFace(Mesh& mesh, Mesh const& face, glm::vec3 offset, glm::vec3 scale)
{
    for (auto const& pos : face.positions) {
        mesh.positions.emplace_back((pos + offset) * scale);
    }
    mesh.normals.insert(mesh.normals.end(), face.normals.begin(), face.normals.end());
}
