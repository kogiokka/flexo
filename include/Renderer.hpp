#ifndef RENDERER_H
#define RENDERER_H

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"

#include "Graphics.hpp"
#include "bindable/IndexBuffer.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/Texture2D.hpp"
#include "bindable/VertexBuffer.hpp"
#include "drawable/Drawable.hpp"
#include "drawable/LatticeEdge.hpp"
#include "drawable/LatticeFace.hpp"
#include "drawable/LatticeVertex.hpp"
#include "drawable/LightSource.hpp"
#include "drawable/PolygonalModel.hpp"
#include "drawable/VolumetricModel.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <vector>

using BufferType = int;
using ShaderType = int;
using RenderOption = int;

enum RenderOption_ : int {
    RenderOption_Model = 1 << 0,
    RenderOption_LatticeVertex = 1 << 1,
    RenderOption_LatticeEdge = 1 << 2,
    RenderOption_LatticeFace = 1 << 3,
    RenderOption_LightSource = 1 << 4,
};

class Renderer
{
    Graphics gfx_;

    std::unique_ptr<PolygonalModel> polyModel_;
    std::unique_ptr<LightSource> lightSource_;
    std::unique_ptr<VolumetricModel> volModel_;
    std::unique_ptr<LatticeVertex> latticeVert_;
    std::unique_ptr<LatticeEdge> latticeEdge_;
    std::unique_ptr<LatticeFace> latticeFace_;

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
