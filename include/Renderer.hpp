#ifndef RENDERER_H
#define RENDERER_H

#include <memory>

#include "Attachable.hpp"
#include "Graphics.hpp"
#include "Mesh.hpp"
#include "drawable/Drawable.hpp"
#include "drawable/LatticeEdge.hpp"
#include "drawable/LatticeFace.hpp"
#include "drawable/LatticeVertex.hpp"
#include "drawable/LightSource.hpp"
#include "drawable/PolygonalModel.hpp"
#include "drawable/VolumetricModel.hpp"

class WatermarkingProject;

class Renderer : public AttachableBase
{
public:
    static Renderer& Get(WatermarkingProject& project);
    static Renderer const& Get(WatermarkingProject const& project);

    Renderer(int width, int height);
    void Render();
    void SetCameraView(BoundingBox const& box);
    void LoadPolygonalModel(Mesh const& mesh);
    void LoadLattice();
    void LoadVolumetricModel();
    Camera& GetCamera();

private:
    Graphics m_gfx;

    std::unique_ptr<PolygonalModel> m_polyModel;
    std::unique_ptr<LightSource> m_lightSource;
    std::unique_ptr<VolumetricModel> m_volModel;
    std::unique_ptr<LatticeVertex> m_latticeVert;
    std::unique_ptr<LatticeEdge> m_latticeEdge;
    std::unique_ptr<LatticeFace> m_latticeFace;
};

#endif
