#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <functional>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Attachable.hpp"
#include "Camera.hpp"

#define STD140_ALIGN alignas(sizeof(float) * 4)

enum class InputClassification {
    PerVertex,
    PerInstance,
};

enum class InputFormat {
    Float2,
    Float3,
    Uint2,
    Uint3,
    Uint2Norm,
    Uint3Norm,
};

enum class ShaderStage {
    Vert,
    Tesc,
    Tese,
    Geom,
    Frag,
    Comp,
};

enum class FillMode {
    WireFrame,
    Solid,
};

enum class CullMode {
    None,
    Front,
    Back,
};

typedef enum {
    Filter_MinMagNearest_NoMip = 0x0,
    Filter_MinMagLinear_NoMip = 0x1,
    Filter_MinMagNearest_MipLinear = 0x2,
    Filter_MinMagNearest_MipNearest = 0x3,
    Filter_MinMagLinear_MipNearest = 0x4,
    Filter_MinMagLinear_MipLinear = 0x5,
} Filter;

typedef enum {
    TextureCoordinatesMode_Wrap = GL_REPEAT,
    TextureCoordinatesMode_Mirror = GL_MIRRORED_REPEAT,
    TextureCoordinatesMode_Clamp = GL_CLAMP_TO_EDGE,
    TextureCoordinatesMode_Border = GL_CLAMP_TO_BORDER,
} TextureCoordinatesMode;

typedef enum {
    Blend_Zero = GL_ZERO,
    Blend_One = GL_ONE,
    Blend_SrcAlpha = GL_SRC_ALPHA,
    Blend_DstAlpha = GL_DST_ALPHA,
    Blend_SrcColor = GL_SRC_COLOR,
    Blend_DstColor = GL_DST_COLOR,
    Blend_OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
    Blend_OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
    Blend_OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
    Blend_OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
} Blend;

typedef enum {
    BlendEq_Add = GL_FUNC_ADD,
    BlendEq_Subtract = GL_FUNC_SUBTRACT,
    BlendEq_ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT,
    BlendEq_Min = GL_MIN,
    BlendEq_Max = GL_MAX,
} BlendEq;

struct InputElementDesc {
    GLchar const* semanticName;
    InputFormat format;
    GLuint inputSlot;
    GLuint byteOffset;
    InputClassification inputSlotClass;
    GLuint instanceDataStepRate;
};

struct BufferDesc {
    GLenum target;
    GLenum usage;
    GLsizeiptr byteWidth;
    GLsizei stride;
};

struct Texture2dDesc {
    GLsizei width;
    GLsizei height;
    GLenum textureFormat;
    GLenum pixelFormat;
    GLenum dataType;
};

struct SamplerDesc {
    Filter filter;
    TextureCoordinatesMode coordinateS;
    TextureCoordinatesMode coordinateT;
    TextureCoordinatesMode coordinateR;
};

struct RasterizerDesc {
    FillMode fillMode;
    CullMode cullMode;
};

struct BlendDesc {
    bool enable;
    Blend srcRGB;
    Blend dstRGB;
    BlendEq eqRGB;
    Blend srcAlpha;
    Blend dstAlpha;
    BlendEq eqAlpha;
};

struct ResourceData {
    void const* mem;
};

template <typename T>
class GLPtr
{
    T* m_ptr;

public:
    GLPtr()
        : m_ptr(nullptr)
    {
    }
    ~GLPtr()
    {
        delete m_ptr;
    }
    T** operator&()
    {
        return &m_ptr;
    }
    T* Get()
    {
        return m_ptr;
    }
};

class _GLState
{
    using _Op = std::function<void(void)>;
    std::vector<_Op> m_ops;

public:
    void Add(_Op op);
    void Execute() const;
};

using RasterizerState = _GLState;
using BlendState = _GLState;

struct Viewport {
    float x;
    float y;
    float width;
    float height;
    double nearDepth;
    double farDepth;
};

class WatermarkingProject;

class Graphics : public AttachableBase
{
    struct Context {
        GLenum primitive;
        GLenum elementDataType;
        const GLvoid* offsetOfFirstIndex;
    };

    struct GLAttribFormat {
        GLint size;
        GLenum type;
        GLboolean normalized;
    };

    Context m_ctx;
    Camera m_camera;

public:
    static Graphics& Get(WatermarkingProject& project);
    static Graphics const& Get(WatermarkingProject const& project);

    Graphics(int width, int height);
    void CreateInputLayout(GLuint& layout, InputElementDesc const* inputElementDesc, int const numElements,
                           GLuint const programWithInputSignature);
    void CreateBuffer(GLuint& buffer, BufferDesc const& desc, ResourceData const& data);
    void CreateTexture2D(GLuint& texture, GLuint const unit, Texture2dDesc const& desc, ResourceData const& data);
    void CreateSampler(GLuint& sampler, SamplerDesc const& desc);
    void CreateShaderProgram(GLuint& program);
    void CreateProgramPipeline(GLuint& pipeline);
    void CreateSeparableShaderProgram(GLuint& program, ShaderStage stage, std::string const& filename);
    void CreateRasterizerState(RasterizerDesc const& desc, RasterizerState** ppState);
    void CreateBlendState(BlendDesc const& desc, BlendState** ppState);
    void AttachShaderStage(GLuint const program, ShaderStage stage, std::string const& filename);
    void LinkShaderProgram(GLuint const program);
    void SetPrimitive(GLenum primitive);
    void SetVertexBuffers(GLuint first, int numBuffers, GLuint const* buffers, GLintptr const* offsets,
                          GLsizei const* strides);
    void SetInputLayout(GLuint const layout);
    void SetIndexBuffer(GLuint buffer, GLenum elementDataType, GLvoid const* offsetOfFirstIndex);
    void SetTexture(GLenum target, GLuint texture, GLuint unit);
    void SetSampler(GLuint unit, GLuint sampler);
    void SetShaderProgram(GLuint program);
    void SetProgramPipeline(GLuint pipeline);
    void SetProgramPipelineStages(GLuint pipeline, GLbitfield stages, GLuint program);
    void SetRasterizerState(RasterizerState const* state);
    void SetBlendState(BlendState const* state);
    void SetViewports(unsigned int numViewports, Viewport* viewports);
    void SetUniformBuffer(GLuint const uniform, GLuint const bindingIndex);

    void DeleteInputLayout(GLuint& layout);
    void DeleteBuffer(GLuint& buffer);
    void DeleteTexture(GLuint& texture);
    void DeleteSampler(GLuint& sampler);
    void DeleteShaderProgram(GLuint& program);
    void DeleteProgramPipeline(GLuint& pipeline);
    void ClearBuffer(float red, float green, float blue) const;

    glm::mat4 GetViewProjectionMatrix() const;
    glm::vec3 GetCameraPosition() const;
    Camera& GetCamera();

    void UpdateBuffer(GLuint const buffer, GLenum target, GLintptr offset, GLsizei byteWidth, void const* data);

    void Draw(GLsizei vertexCount);
    void DrawIndexed(GLsizei indexCount);
    void DrawInstanced(GLsizei vertexCountPerInstance, GLsizei instanceCount);

private:
    int UniformLocation(std::string const& uniformName) const;
    bool IsShaderCompiled(GLuint const shaderObject);
    void CheckProgramStatus(GLuint const programObject);
    std::string SlurpShaderSource(std::string const& filename) const;

    struct Enum {
        static GLAttribFormat Resolve(InputFormat const format);
        static GLenum Resolve(ShaderStage const stage);
    };
};

#endif
