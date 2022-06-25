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

using BufferType = int;
using ShaderType = int;
using RenderOption = int;

enum m_RenderOption : int {
    RenderOption_Model = 1 << 0,
    RenderOption_LatticeVertex = 1 << 1,
    RenderOption_LatticeEdge = 1 << 2,
    RenderOption_LatticeFace = 1 << 3,
    RenderOption_LightSource = 1 << 4,
};

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
    void LoadPolygonalModel();
    void LoadLattice();
    void LoadVolumetricModel();
    Camera& GetCamera();
};

extern RenderOption rendopt;

#endif
