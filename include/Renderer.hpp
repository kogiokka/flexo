#ifndef RENDERER_H
#define RENDERER_H

#include "Camera.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"
#include "VertexArray.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>
#include <vector>

using BufferType = int;
using ShaderType = int;
using RenderOption = int;

enum ShaderType_ : int {
    ShaderType_Default = 0,
    ShaderType_VolumetricModel,
    ShaderType_LatticeEdge,
    ShaderType_LatticeVertex,
    ShaderType_LightSource,
    ShaderType_Last = ShaderType_LightSource,
};

enum BufferType_ : int {
    BufferType_Surface = 0,
    BufferType_Cube,
    BufferType_UVSphere,
    BufferType_VolumetricModel,
    BufferType_LatticePositions,
    BufferType_LatticeEdge,
    BufferType_LatticeFace,
    BufferType_Last = BufferType_LatticeFace,
};

enum RenderOption_ : int {
    RenderOption_Model = 1 << 0,
    RenderOption_LatticeVertex = 1 << 1,
    RenderOption_LatticeEdge = 1 << 2,
    RenderOption_LatticeFace = 1 << 3,
    RenderOption_LightSource = 1 << 4,
};

class Renderer
{
    int width_;
    int height_;
    VertexArray vao_;
    std::array<GLuint, BufferType_Last + 1> buffers_;
    std::array<Shader, ShaderType_Last + 1> shaders_;
    Camera camera_;

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
