#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "OpenGLCanvas.hpp"
#include "Project.hpp"
#include "Renderer.hpp"
#include "World.hpp"
#include "common/Logger.hpp"

// Register factory: Renderer
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<Renderer> {
        auto const& canvas = OpenGLCanvas::Get(project);
        wxSize const size = canvas.GetClientSize() * canvas.GetContentScaleFactor();
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
    , m_polyModel(nullptr)
    , m_lightSource(nullptr)
    , m_volModel(nullptr)
    , m_latticeVert(nullptr)
    , m_latticeEdge(nullptr)
    , m_latticeFace(nullptr)
{
    m_lightSource = std::make_unique<LightSource>(m_gfx, world.uvsphere);
}

void Renderer::Render()
{
    if (!world.theModel) {
        return;
    }
    if (world.theModel->HasNormals()) { // FIXME Is this reliable?
        m_polyModel->Update(m_gfx);
        m_polyModel->Draw(m_gfx);
    } else {
        glEnable(GL_CULL_FACE); // FIXME: VertexArrray bindable?
        glCullFace(GL_BACK);
        m_volModel->Update(m_gfx);
        m_volModel->Draw(m_gfx);
        glDisable(GL_CULL_FACE);
    }

    if (m_latticeVert) {
        m_latticeVert->Update(m_gfx);
        m_latticeVert->Draw(m_gfx);
    }
    if (m_lightSource) {
        m_lightSource->Update(m_gfx);
        m_lightSource->Draw(m_gfx);
    }
    if (m_latticeEdge) {
        m_latticeEdge->Update(m_gfx);
        m_latticeEdge->Draw(m_gfx);
    }
    if (m_latticeFace) {
        m_latticeFace->Update(m_gfx);
        m_latticeFace->Draw(m_gfx);
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

void Renderer::LoadPolygonalModel(Mesh const& mesh)
{
    m_polyModel = std::make_unique<PolygonalModel>(m_gfx, mesh);
}

void Renderer::LoadLattice()
{
    m_latticeVert = std::make_unique<LatticeVertex>(m_gfx, world.uvsphere, world.neurons);
    m_latticeEdge = std::make_unique<LatticeEdge>(m_gfx, world.neurons);
    m_latticeFace = std::make_unique<LatticeFace>(m_gfx, world.latticeMesh);
}

void Renderer::LoadVolumetricModel()
{
    m_volModel = std::make_unique<VolumetricModel>(m_gfx, world.cube, *world.theModel);
}

Camera& Renderer::GetCamera()
{
    return m_gfx.GetCamera();
}
