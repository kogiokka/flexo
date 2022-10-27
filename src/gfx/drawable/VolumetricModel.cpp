#include <utility>
#include <vector>

#include "Vertex.hpp"
#include "World.hpp"
#include "assetlib/STL/STLImporter.hpp"
#include "gfx/Task.hpp"
#include "gfx/bindable/InputLayout.hpp"
#include "gfx/bindable/Primitive.hpp"
#include "gfx/bindable/RasterizerState.hpp"
#include "gfx/bindable/Sampler.hpp"
#include "gfx/bindable/TextureManager.hpp"
#include "gfx/bindable/TransformUniformBuffer.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/bindable/VertexBuffer.hpp"
#include "gfx/bindable/program/FragmentShaderProgram.hpp"
#include "gfx/bindable/program/VertexShaderProgram.hpp"
#include "gfx/drawable/VolumetricModel.hpp"

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

VolumetricModel::VolumetricModel(Graphics& gfx, RVL& rvl)
    : m_ub {}
{
    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, 0, GLWRInputClassification_PerVertex, 0 },
        { "normal", GLWRFormat_Float3, 1, 0, GLWRInputClassification_PerVertex, 0 },
        { "textureCoord", GLWRFormat_Float2, 2, 0, GLWRInputClassification_PerVertex, 0 },
    };

    CreateMesh(rvl);

    m_isVisible = true;

    m_ub.vert.viewPos = gfx.GetCameraPosition();
    m_ub.vert.light.position = gfx.GetCameraPosition();
    m_ub.vert.light.ambient = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.vert.light.diffusion = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.vert.light.specular = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.vert.material.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.vert.material.diffusion = glm::vec3(0.6f, 0.6f, 0.6f);
    m_ub.vert.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.vert.material.shininess = 256.0f;
    m_ub.vert.isWatermarked = false;

    GLWRSamplerDesc samplerDesc;
    samplerDesc.coordinateS = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.coordinateT = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.coordinateR = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.filter = GLWRFilter_MinMagNearest_NoMip;

    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, m_mesh.positions, 0));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, m_mesh.normals, 1));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, m_mesh.textureCoords, 2));

    Task draw;
    draw.mDrawable = this;

    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/VolumetricModel.vert");
    draw.AddBindable(vs);
    draw.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/VolumetricModel.frag"));
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    draw.AddBindable(std::make_shared<Bind::TransformUniformBuffer>(gfx, glm::mat4(1.0f)));
    draw.AddBindable(std::make_shared<Bind::UniformBuffer<UniformBlock>>(gfx, m_ub, 1));
    draw.AddBindable(
        std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_Back }));
    draw.AddBindable(Bind::TextureManager::Resolve(gfx, world.imagePath.c_str(), 0));
    draw.AddBindable(Bind::TextureManager::Resolve(gfx, "res/images/blank.png", 1));
    draw.AddBindable(std::make_shared<Bind::Sampler>(gfx, samplerDesc, 0));
    draw.AddBindable(std::make_shared<Bind::Sampler>(gfx, samplerDesc, 1));
    AddTask(draw);
}

VolumetricModel::~VolumetricModel()
{
}

// FIXME
void VolumetricModel::ChangeTexture(Graphics& gfx, char const* filename)
{
    bool (*const FindTexture)(std::shared_ptr<Bind::Bindable>&)
        = [](std::shared_ptr<Bind::Bindable>& bind) { return (dynamic_cast<Bind::Texture2D*>(bind.get()) != nullptr); };

    auto& taskBinds = m_tasks.front().mBinds;
    taskBinds.erase(std::remove_if(taskBinds.begin(), taskBinds.end(), FindTexture), taskBinds.end());
    m_tasks.front().AddBindable(Bind::TextureManager::Resolve(gfx, filename, 0));
}

void VolumetricModel::Update(Graphics& gfx)
{
    m_ub.vert.viewPos = gfx.GetCameraPosition();
    m_ub.vert.light.position = gfx.GetCameraPosition();
    m_ub.vert.isWatermarked = world.isWatermarked;

    // FIXME Need to rework UniformBuffer creation/update
    auto const& taskBinds = m_tasks.front().mBinds;
    for (auto it = taskBinds.begin(); it != taskBinds.end(); it++) {
        Bind::UniformBuffer<UniformBlock>* ub = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
        if (ub != nullptr) {
            ub->Update(gfx, m_ub);
        }
    }
}

std::string VolumetricModel::GetName() const
{
    return "Volumetric Model";
}

void VolumetricModel::CreateMesh(RVL& rvl)
{
    int x, y, z;
    const unsigned char* data;
    const void* buf;
    glm::vec3 dims;

    RVLenum primitive, endian;
    rvl_get_volumetric_format(&rvl, &x, &y, &z, &primitive, &endian);
    rvl_get_data_buffer(&rvl, &buf);
    rvl_get_voxel_dims(&rvl, &dims.x, &dims.y, &dims.z);

    world.numVxVerts.resize(x * y * z);

    data = static_cast<const unsigned char*>(buf);
    const unsigned char model = 255;
    const unsigned char air = 0;
    for (int i = 0; i < z; i++) {
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < x; k++) {
                int index = k + j * x + i * x * y;
                glm::vec3 offset { k, j, i };
                if (data[index] == model) {
                    if ((k + 1 == x) || data[index + 1] == air) {
                        AddFace(VOXEL.face.xp, offset, dims);
                        world.numVxVerts[index] += 6;
                    }
                    if ((j + 1 == y) || (data[index + x] == air)) {
                        AddFace(VOXEL.face.yp, offset, dims);
                        world.numVxVerts[index] += 6;
                    }
                    if ((i + 1 == z) || (data[index + x * y] == air)) {
                        AddFace(VOXEL.face.zp, offset, dims);
                        world.numVxVerts[index] += 6;
                    }
                    if ((k == 0) || (data[index - 1] == air)) {
                        AddFace(VOXEL.face.xn, offset, dims);
                        world.numVxVerts[index] += 6;
                    }
                    if ((j == 0) || (data[index - x] == air)) {
                        AddFace(VOXEL.face.yn, offset, dims);
                        world.numVxVerts[index] += 6;
                    }
                    if ((i == 0) || (data[index - x * y] == air)) {
                        AddFace(VOXEL.face.zn, offset, dims);
                        world.numVxVerts[index] += 6;
                    }
                }
            }
        }
    }

    m_mesh.textureCoords = std::vector<glm::vec2>(m_mesh.positions.size(), glm::vec2(0.0f, 0.0f));
    world.theModel->textureCoords = m_mesh.textureCoords;
}

void VolumetricModel::AddFace(Mesh const& face, glm::vec3 offset, glm::vec3 scale)
{
    for (auto const& pos : face.positions) {
        m_mesh.positions.emplace_back((pos + offset) * scale);
    }
    m_mesh.normals.insert(m_mesh.normals.end(), face.normals.begin(), face.normals.end());
}
