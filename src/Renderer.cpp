#include "Renderer.hpp"
#include "World.hpp"
#include "common/Logger.hpp"
#include <cstdlib>

RenderOption rendopt = RenderOption_Model | RenderOption_LatticeEdge | RenderOption_LatticeVertex;

Renderer::Renderer(int width, int height)
    : gfx_(width, height)
    , polyModel_(nullptr)
    , lightSource_(nullptr)
    , volModel_(nullptr)
    , latticeVert_(nullptr)
    , latticeEdge_(nullptr)
    , latticeFace_(nullptr)
{
    lightSource_ = std::make_unique<LightSource>(gfx_, world.uvsphere);
}

void Renderer::Render()
{
    if (world.polyModel == nullptr && world.volModel == nullptr) {
        return;
    }

    if (rendopt & RenderOption_LightSource) {
        lightSource_->Update(gfx_);
        lightSource_->Draw(gfx_);
    }

    if (rendopt & RenderOption_LatticeVertex) {
        latticeVert_->Update(gfx_);
        latticeVert_->Draw(gfx_);
    }

    if (rendopt & RenderOption_Model) {
        if (world.polyModel && polyModel_) {
            polyModel_->Update(gfx_);
            polyModel_->Draw(gfx_);
        } else if (world.volModel != nullptr) {
            glEnable(GL_CULL_FACE); // FIXME: VertexArrray bindable?
            glCullFace(GL_BACK);
            volModel_->Update(gfx_);
            volModel_->Draw(gfx_);
            glDisable(GL_CULL_FACE);
        }
    }

    if (rendopt & RenderOption_LatticeEdge) {
        latticeEdge_->Update(gfx_);
        latticeEdge_->Draw(gfx_);
    }

    if (rendopt & RenderOption_LatticeFace) {
        latticeFace_->Update(gfx_);
        latticeFace_->Draw(gfx_);
    }
}

void Renderer::LoadPolygonalModel()
{
    assert(world.polyModel);
    polyModel_ = std::make_unique<PolygonalModel>(gfx_, *world.polyModel);
}

void Renderer::LoadLattice()
{
    latticeVert_ = std::make_unique<LatticeVertex>(gfx_, world.uvsphere);
    latticeEdge_ = std::make_unique<LatticeEdge>(gfx_, world.neurons);
    latticeFace_ = std::make_unique<LatticeFace>(gfx_, world.latticeMesh);
}

void Renderer::LoadVolumetricModel()
{
    volModel_ = std::make_unique<VolumetricModel>(gfx_, world.cube);
}

Camera& Renderer::GetCamera()
{
    return gfx_.GetCamera();
}
