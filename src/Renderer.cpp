#include "Renderer.hpp"
#include "World.hpp"

RenderOption rendopt = RenderOption_Model | RenderOption_LatticeEdge | RenderOption_LatticeVertex;

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
    if (world.polyModel == nullptr && world.volModel == nullptr) {
        return;
    }

    if (rendopt & RenderOption_LightSource) {
        m_lightSource->Update(m_gfx);
        m_lightSource->Draw(m_gfx);
    }

    if (rendopt & RenderOption_LatticeVertex) {
        m_latticeVert->Update(m_gfx);
        m_latticeVert->Draw(m_gfx);
    }

    if (rendopt & RenderOption_Model) {
        if (world.polyModel && m_polyModel) {
            m_polyModel->Update(m_gfx);
            m_polyModel->Draw(m_gfx);
        } else if (world.volModel != nullptr) {
            glEnable(GL_CULL_FACE); // FIXME: VertexArrray bindable?
            glCullFace(GL_BACK);
            m_volModel->Update(m_gfx);
            m_volModel->Draw(m_gfx);
            glDisable(GL_CULL_FACE);
        }
    }

    if (rendopt & RenderOption_LatticeEdge) {
        m_latticeEdge->Update(m_gfx);
        m_latticeEdge->Draw(m_gfx);
    }

    if (rendopt & RenderOption_LatticeFace) {
        m_latticeFace->Update(m_gfx);
        m_latticeFace->Draw(m_gfx);
    }
}

void Renderer::LoadPolygonalModel()
{
    assert(world.polyModel);
    m_polyModel = std::make_unique<PolygonalModel>(m_gfx, *world.polyModel);
}

void Renderer::LoadLattice()
{
    m_latticeVert = std::make_unique<LatticeVertex>(m_gfx, world.uvsphere);
    m_latticeEdge = std::make_unique<LatticeEdge>(m_gfx, world.neurons);
    m_latticeFace = std::make_unique<LatticeFace>(m_gfx, world.latticeMesh);
}

void Renderer::LoadVolumetricModel()
{
    m_volModel = std::make_unique<VolumetricModel>(m_gfx, world.cube);
}

Camera& Renderer::GetCamera()
{
    return m_gfx.GetCamera();
}
