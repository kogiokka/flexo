#include "Renderer.hpp"
#include "Shader.hpp"
#include "World.hpp"
#include "common/Logger.hpp"
#include <cstdlib>

RenderOption rendopt = RenderOption_Model | RenderOption_LatticeEdge | RenderOption_LatticeVertex;

Renderer::Renderer(int width, int height)
    : gfx_(width, height)
    , polyModel_(nullptr)
    , lightSource_(nullptr)
    , volModel_(nullptr)
    , latticeFace_(nullptr)
{
    AttribFormat format;
    format.count = 3;
    format.type = GL_FLOAT;
    format.normalized = GL_FALSE;
    vao_.Bind();
    vao_.AddAttribFormat(VertexAttrib_Position, format); // 0
    vao_.AddAttribFormat(VertexAttrib_Normal, format); // 1
    vao_.AddAttribFormat(VertexAttrib_Translation, format); // 2
    vao_.AddAttribFormat(VertexAttrib_TextureCoord, { 2, GL_FLOAT, GL_FALSE }); // 3
    vao_.Enable(VertexAttrib_Position);
    vao_.Enable(VertexAttrib_Normal);
    vao_.Enable(VertexAttrib_TextureCoord);

    vaoTemp_.Bind();
    vaoTemp_.AddAttribFormat(VertexAttrib_Position, format); // 0
    vaoTemp_.AddAttribFormat(VertexAttrib_Normal, format); // 1
    vaoTemp_.AddAttribFormat(2, { 2, GL_FLOAT, GL_FALSE }); // Texture coordinates
    vaoTemp_.Enable(VertexAttrib_Position);
    vaoTemp_.Enable(VertexAttrib_Normal);
    vaoTemp_.Enable(2);

    vao_.Bind();

    lightSource_ = std::make_unique<LightSource>(gfx_, world.uvsphere);
}

void Renderer::Render()
{
    if (world.polyModel == nullptr && world.volModel == nullptr) {
        return;
    }

    if (rendopt & RenderOption_LightSource) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Disable(VertexAttrib_Normal);
        vao_.Disable(VertexAttrib_TextureCoord);
        vao_.Disable(VertexAttrib_Translation);
        lightSource_->Update(gfx_);
        lightSource_->Draw(gfx_);
    }

    if (rendopt & RenderOption_LatticeVertex) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Enable(VertexAttrib_Normal);
        vao_.Disable(VertexAttrib_TextureCoord);
        vao_.Enable(VertexAttrib_Translation);
        latticeVert_->Update(gfx_);
        latticeVert_->Draw(gfx_);
    }

    if (rendopt & RenderOption_Model) {
        if (world.polyModel && polyModel_) {
            vao_.Enable(VertexAttrib_Position);
            vao_.Enable(VertexAttrib_Normal);
            vao_.Disable(VertexAttrib_TextureCoord);
            vao_.Disable(VertexAttrib_Translation);
            polyModel_->Update(gfx_);
            polyModel_->Draw(gfx_);
        } else if (world.volModel != nullptr) {
            glEnable(GL_CULL_FACE); // FIXME: VertexArrray bindable?
            glCullFace(GL_BACK);
            vao_.Enable(VertexAttrib_Position);
            vao_.Enable(VertexAttrib_Normal);
            vao_.Enable(VertexAttrib_TextureCoord);
            vao_.Enable(VertexAttrib_Translation);
            volModel_->Update(gfx_);
            volModel_->Draw(gfx_);
            glDisable(GL_CULL_FACE);
        }
    }

    if (rendopt & RenderOption_LatticeEdge) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Disable(VertexAttrib_Normal);
        vao_.Disable(VertexAttrib_TextureCoord);
        vao_.Disable(VertexAttrib_Translation);
        latticeEdge_->Update(gfx_);
        latticeEdge_->Draw(gfx_);
    }

    if (rendopt & RenderOption_LatticeFace) {
        vaoTemp_.Bind();
        latticeFace_->Update(gfx_);
        latticeFace_->Draw(gfx_);
        vao_.Bind();
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
