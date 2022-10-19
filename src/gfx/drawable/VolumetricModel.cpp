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

// ================================================================================
#include <iostream>

VolumetricModel::VolumetricModel(Graphics& gfx, glm::ivec3 resolution, uint8_t const* data)
    : m_ub {}
    , m_resolution(resolution)
    , m_data(data)
{
    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float3, 0, 0, GLWRInputClassification_PerVertex, 0 },
        { "normal", GLWRFormat_Float3, 1, 0, GLWRInputClassification_PerVertex, 0 },
        { "textureCoord", GLWRFormat_Float2, 2, 0, GLWRInputClassification_PerVertex, 0 },
    };

    Mesh const mesh = CreateMesh();
    std::cout << "Mesh positions: " << mesh.positions.size() << std::endl;
    std::cout << "Mesh normal : " << mesh.normals.size() << std::endl;
    std::cout << "Mesh texture coordinates: " << mesh.textureCoords.size() << std::endl;

    m_isVisible = true;

    m_ub.vert.viewPos = gfx.GetCameraPosition();
    m_ub.vert.light.position = gfx.GetCameraPosition();
    m_ub.vert.light.ambient = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.vert.light.diffusion = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.vert.light.specular = glm::vec3(0.8f, 0.8f, 0.8f);
    m_ub.vert.material.ambient = glm::vec3(0.0f, 0.3f, 0.0f);
    m_ub.vert.material.diffusion = glm::vec3(0.0f, 0.6f, 0.0f);
    m_ub.vert.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    m_ub.vert.material.shininess = 256.0f;
    m_ub.vert.isWatermarked = false;

    GLWRSamplerDesc samplerDesc;
    samplerDesc.coordinateS = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.coordinateT = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.coordinateR = GLWRTextureCoordinatesMode_Wrap;
    samplerDesc.filter = GLWRFilter_MinMagNearest_NoMip;

    AddBind(std::make_shared<Bind::Primitive>(gfx, GL_TRIANGLES));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, mesh.positions, 0));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, mesh.normals, 1));
    AddBind(std::make_shared<Bind::VertexBuffer>(gfx, mesh.textureCoords, 2));

    Task draw;
    draw.mDrawable = this;

    glm::vec3 vxScale(world.vxDim[0], world.vxDim[1], world.vxDim[2]);
    auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/VolumetricModel.vert");
    draw.AddBindable(vs);
    draw.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/VolumetricModel.frag"));
    draw.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
    draw.AddBindable(std::make_shared<Bind::TransformUniformBuffer>(gfx, glm::scale(glm::mat4(1.0f), vxScale)));
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

    // for (auto it = m_binds.begin(); it != m_binds.end(); it++) {
    //     Bind::VertexBuffer* vb = dynamic_cast<Bind::VertexBuffer*>(it->get());
    //     if ((vb != nullptr) && (vb->GetStartAttrib() == 2)) {
    //         vb->Update(gfx, world.theModel->textureCoords);
    //     }
    // }

    // FIXME Need to rework UniformBuffer creation/update
    auto const& taskBinds = m_tasks.front().mBinds;
    for (auto it = taskBinds.begin(); it != taskBinds.end(); it++) {
        Bind::UniformBuffer<UniformBlock>* ub = dynamic_cast<Bind::UniformBuffer<UniformBlock>*>(it->get());
        if (ub != nullptr) {
            ub->Update(gfx, m_ub);
        }
    }
}

Mesh VolumetricModel::CreateMesh()
{
    int x = m_resolution[0];
    int y = m_resolution[1];
    int z = m_resolution[2];

    Mesh mesh;
    static STLImporter stlimp;
    static Mesh const xpos = stlimp.ReadFile("res/models/voxel/cube-x-pos.stl");
    static Mesh const ypos = stlimp.ReadFile("res/models/voxel/cube-y-pos.stl");
    static Mesh const zpos = stlimp.ReadFile("res/models/voxel/cube-z-pos.stl");
    static Mesh const xneg = stlimp.ReadFile("res/models/voxel/cube-x-neg.stl");
    static Mesh const yneg = stlimp.ReadFile("res/models/voxel/cube-y-neg.stl");
    static Mesh const zneg = stlimp.ReadFile("res/models/voxel/cube-z-neg.stl");

    for (int i = 0; i < z; i++) {
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < x; k++) {
                uint8_t value = m_data[k + j * x + i * x * y];
                if (value == 0)
                    continue;

                Mesh face;
                if (value & VoxelExposedDir_X_Pos) {
                    face = xpos;
                    for (auto& p : face.positions) {
                        p += glm::vec3(k, j, i);
                    }
                    mesh.positions.insert(mesh.positions.end(), face.positions.begin(), face.positions.end());
                    mesh.normals.insert(mesh.normals.end(), xpos.normals.begin(), xpos.normals.end());
                }
                if (value & VoxelExposedDir_Y_Pos) {
                    face = ypos;
                    for (auto& p : face.positions) {
                        p += glm::vec3(k, j, i);
                    }
                    mesh.positions.insert(mesh.positions.end(), face.positions.begin(), face.positions.end());
                    mesh.normals.insert(mesh.normals.end(), ypos.normals.begin(), ypos.normals.end());
                }
                if (value & VoxelExposedDir_Z_Pos) {
                    face = zpos;
                    for (auto& p : face.positions) {
                        p += glm::vec3(k, j, i);
                    }
                    mesh.positions.insert(mesh.positions.end(), face.positions.begin(), face.positions.end());
                    mesh.normals.insert(mesh.normals.end(), zpos.normals.begin(), zpos.normals.end());
                }
                if (value & VoxelExposedDir_X_Neg) {
                    face = xneg;
                    for (auto& p : face.positions) {
                        p += glm::vec3(k, j, i);
                    }
                    mesh.positions.insert(mesh.positions.end(), face.positions.begin(), face.positions.end());
                    mesh.normals.insert(mesh.normals.end(), xneg.normals.begin(), xneg.normals.end());
                }
                if (value & VoxelExposedDir_Y_Neg) {
                    face = yneg;
                    for (auto& p : face.positions) {
                        p += glm::vec3(k, j, i);
                    }
                    mesh.positions.insert(mesh.positions.end(), face.positions.begin(), face.positions.end());
                    mesh.normals.insert(mesh.normals.end(), yneg.normals.begin(), yneg.normals.end());
                }
                if (value & VoxelExposedDir_Z_Neg) {
                    face = zneg;
                    for (auto& p : face.positions) {
                        p += glm::vec3(k, j, i);
                    }
                    mesh.positions.insert(mesh.positions.end(), face.positions.begin(), face.positions.end());
                    mesh.normals.insert(mesh.normals.end(), zneg.normals.begin(), zneg.normals.end());
                }
            }
        }
    }

    mesh.textureCoords = std::vector<glm::vec2>(mesh.positions.size(), glm::vec2(0.0f, 0.0f));

    return mesh;
}

std::string VolumetricModel::GetName() const
{
    return "Volumetric Model";
}
