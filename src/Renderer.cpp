#include "Renderer.hpp"
#include "World.hpp"

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

    if (world.renderFlags & RenderFlag_DrawLightSource) {
        m_lightSource->Update(m_gfx);
        m_lightSource->Draw(m_gfx);
    }

    if (world.renderFlags & RenderFlag_DrawLatticeVertex) {
        m_latticeVert->Update(m_gfx);
        m_latticeVert->Draw(m_gfx);
    }

    if (world.renderFlags & RenderFlag_DrawModel) {
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
    }

    if (world.renderFlags & RenderFlag_DrawLatticeEdge) {
        m_latticeEdge->Update(m_gfx);
        m_latticeEdge->Draw(m_gfx);
    }

    if (world.renderFlags & RenderFlag_DrawLatticeFace) {
        m_latticeFace->Update(m_gfx);
        m_latticeFace->Draw(m_gfx);
    }
}

void Renderer::LoadPolygonalModel(Mesh const& mesh)
{
    m_polyModel = std::make_unique<PolygonalModel>(m_gfx, mesh);
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
