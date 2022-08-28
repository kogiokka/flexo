#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Project.hpp"
#include "Renderer.hpp"
#include "World.hpp"
#include "common/Logger.hpp"
#include "pane/SceneViewportPane.hpp"

// Register factory: Renderer
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<Renderer> {
        auto const& viewport = SceneViewportPane::Get(project);
        wxSize const size = viewport.GetClientSize() * viewport.GetContentScaleFactor();
        return std::make_shared<Renderer>(size.x, size.y);
    }
};

Renderer& Renderer::Get(WatermarkingProject& project)
{
    return project.AttachedObjects::Get<Renderer>(factoryKey);
}

Renderer const& Renderer::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

Renderer::Renderer(int width, int height)
    : m_gfx(width, height)
{
    m_objects["Light"].push_back(std::make_shared<LightSource>(m_gfx, world.uvsphere));
}

void Renderer::Render()
{
    for (auto const& [name, objs] : m_objects) {
        for (auto const& obj : objs) {
            obj->Update(m_gfx);
            obj->Draw(m_gfx);
        }
    }
}

void Renderer::SetCameraView(BoundingBox const& box)
{
    auto const& [max, min] = box;
    glm::vec3 center = (max + min) * 0.5f;

    auto& camera = m_gfx.GetCamera();
    camera.center = center;
    Logger::info("Camera looking at: %s", glm::to_string(center).c_str());

    camera.UpdateViewCoord();

    glm::vec3 diff = max - min;
    float size = diff.x;
    if (size < diff.y) {
        size = diff.y;
    }
    if (size < diff.z) {
        size = diff.z;
    }
    camera.volumeSize = size;
}

void Renderer::LoadLattice()
{
    LatticeVertex* vert = nullptr;
    LatticeEdge* edge = nullptr;
    LatticeFace* face = nullptr;

    for (auto& [name, objs] : m_objects) {
        vert = dynamic_cast<LatticeVertex*>(objs.front().get());
        edge = dynamic_cast<LatticeEdge*>(objs.front().get());
        face = dynamic_cast<LatticeFace*>(objs.front().get());
        if (vert) {
            objs.front() = std::make_shared<LatticeVertex>(m_gfx, world.uvsphere, world.neurons);
        }
        if (edge) {
            objs.front() = std::make_shared<LatticeEdge>(m_gfx, world.neurons);
        }
        if (face) {
            objs.front() = std::make_shared<LatticeFace>(m_gfx, world.latticeMesh);
        }
    }

    // These three drawables exist at the same time
    if (!vert && !edge && !face) {
        m_objects.push_back(std::make_shared<LatticeVertex>(m_gfx, world.uvsphere, world.neurons));
        m_objects.push_back(std::make_shared<LatticeEdge>(m_gfx, world.neurons));
        m_objects.push_back(std::make_shared<LatticeFace>(m_gfx, world.latticeMesh));
    }
}

void Renderer::SubmitPolygonalModel(Mesh const& mesh)
{
    m_objects.push_back(std::make_shared<PolygonalModel>(m_gfx, mesh));
}

void Renderer::SubmitVolumetricModel(Mesh const& instanceMesh, Mesh const& perInstanceData)
{
    m_objects.push_back(std::make_shared<VolumetricModel>(m_gfx, instanceMesh, perInstanceData));
}

Camera& Renderer::GetCamera()
{
    return m_gfx.GetCamera();
}

std::unordered_map<std::string, std::vector<std::shared_ptr<DrawableBase>>> const& Renderer::GetDrawables() const
{
    return m_objects;
}
