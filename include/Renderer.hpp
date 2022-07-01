#ifndef RENDERER_H
#define RENDERER_H

#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "drawable/Drawable.hpp"
#include "drawable/LatticeEdge.hpp"
#include "drawable/LatticeFace.hpp"
#include "drawable/LatticeVertex.hpp"
#include "drawable/LightSource.hpp"
#include "drawable/PolygonalModel.hpp"
#include "drawable/VolumetricModel.hpp"

class Renderer
{
    Graphics m_gfx;

    std::unique_ptr<PolygonalModel> m_polyModel;
    std::unique_ptr<LightSource> m_lightSource;
    std::unique_ptr<VolumetricModel> m_volModel;
    std::unique_ptr<LatticeVertex> m_latticeVert;
    std::unique_ptr<LatticeEdge> m_latticeEdge;
    std::unique_ptr<LatticeFace> m_latticeFace;

public:
    Renderer(int width, int height);
    void Render();
    void LoadPolygonalModel(Mesh const& mesh);
    void LoadLattice();
    void LoadVolumetricModel();
    Camera& GetCamera();
};

#endif
