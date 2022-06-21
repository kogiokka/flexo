#ifndef RENDERER_H
#define RENDERER_H

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"
#include "VertexArray.hpp"

#include "Graphics.hpp"
#include "bindable/IndexBuffer.hpp"
#include "bindable/Primitive.hpp"
#include "bindable/Texture2D.hpp"
#include "bindable/VertexBuffer.hpp"
#include "drawable/Drawable.hpp"
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

enum ShaderType_ : int {
    ShaderType_LatticeVertex,
    ShaderType_LatticeEdge,
    ShaderType_LatticeFace,
    ShaderType_Last = ShaderType_LatticeFace,
};

enum BufferType_ : int {
    BufferType_Surface = 0,
    BufferType_Cube,
    BufferType_UVSphere,
    BufferType_VolumetricModel_Translation,
    BufferType_VolumetricModel_TextureCoord,
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
    VertexArray vao_;
    std::array<GLuint, BufferType_Last + 1> buffers_;
    std::array<Shader, ShaderType_Last + 1> shaders_;

    Graphics gfx_;

    GLuint tex_;

    std::vector<VertexPN> cubeBuf_;
    std::vector<VertexPN> uvsphereBuf_;
    std::vector<VertexPNT> latticeMeshBuf_;
    std::unique_ptr<PolygonalModel> polyModel_;
    std::unique_ptr<LightSource> lightSource_;
    std::unique_ptr<VolumetricModel> volModel_;

public:
    Renderer(int width, int height);
    void Render();
    void LoadPolygonalModel();
    void LoadLattice();
    void LoadVolumetricModel();
    Camera& GetCamera();
    void UpdateLatticeMeshBuffer();
    void SetWatermarkTexture();

private:
    void CreateVertexBuffers();
};

extern RenderOption rendopt;
#endif
