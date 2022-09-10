#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <functional>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Attachable.hpp"
#include "Camera.hpp"
#include "gfx/GLWRBuffer.hpp"
#include "gfx/GLWRInputLayout.hpp"
#include "gfx/GLWRRenderTarget.hpp"
#include "gfx/GLWRSampler.hpp"
#include "gfx/GLWRTexture2D.hpp"

#define STD140_ALIGN alignas(sizeof(float) * 4)

typedef enum {
    GLWRInputClassification_PerVertex,
    GLWRInputClassification_PerInstance,
} GLWRInputClassification;

typedef enum {
    GLWRFormat_Float2,
    GLWRFormat_Float3,
    GLWRFormat_Uint2,
    GLWRFormat_Uint3,
    GLWRFormat_Uint2Norm,
    GLWRFormat_Uint3Norm,
} GLWRFormat;

typedef enum {
    GLWRShaderStage_Vert = GL_VERTEX_SHADER,
    GLWRShaderStage_Tesc = GL_TESS_CONTROL_SHADER,
    GLWRShaderStage_Tese = GL_TESS_EVALUATION_SHADER,
    GLWRShaderStage_Geom = GL_GEOMETRY_SHADER,
    GLWRShaderStage_Frag = GL_FRAGMENT_SHADER,
    GLWRShaderStage_Comp = GL_COMPUTE_SHADER,
} GLWRShaderStage;

typedef enum {
    GLWRFillMode_WireFrame,
    GLWRFillMode_Solid,
} GLWRFillMode;

typedef enum {
    GLWRCullMode_None,
    GLWRCullMode_Front,
    GLWRCullMode_Back,
} GLWRCullMode;

typedef enum {
    GLWRFilter_MinMagNearest_NoMip = 0x0,
    GLWRFilter_MinMagLinear_NoMip = 0x1,
    GLWRFilter_MinMagNearest_MipLinear = 0x2,
    GLWRFilter_MinMagNearest_MipNearest = 0x3,
    GLWRFilter_MinMagLinear_MipNearest = 0x4,
    GLWRFilter_MinMagLinear_MipLinear = 0x5,
} GLWRFilter;

typedef enum {
    GLWRTextureCoordinatesMode_Wrap = GL_REPEAT,
    GLWRTextureCoordinatesMode_Mirror = GL_MIRRORED_REPEAT,
    GLWRTextureCoordinatesMode_Clamp = GL_CLAMP_TO_EDGE,
    GLWRTextureCoordinatesMode_Border = GL_CLAMP_TO_BORDER,
} GLWRTextureCoordinatesMode;

typedef enum {
    GLWRBlend_Zero = GL_ZERO,
    GLWRBlend_One = GL_ONE,
    GLWRBlend_SrcAlpha = GL_SRC_ALPHA,
    GLWRBlend_DstAlpha = GL_DST_ALPHA,
    GLWRBlend_SrcColor = GL_SRC_COLOR,
    GLWRBlend_DstColor = GL_DST_COLOR,
    GLWRBlend_OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
    GLWRBlend_OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
    GLWRBlend_OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
    GLWRBlend_OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
} GLWRBlend;

typedef enum {
    GLWRBlendEq_Add = GL_FUNC_ADD,
    GLWRBlendEq_Subtract = GL_FUNC_SUBTRACT,
    GLWRBlendEq_ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT,
    GLWRBlendEq_Min = GL_MIN,
    GLWRBlendEq_Max = GL_MAX,
} GLWRBlendEq;

struct GLWRInputElementDesc {
    GLchar const* semanticName;
    GLWRFormat format;
    GLuint inputSlot;
    GLuint byteOffset;
    GLWRInputClassification inputSlotClass;
    GLuint instanceDataStepRate;
};

struct GLWRBufferDesc {
    GLenum target;
    GLenum usage;
    GLsizeiptr byteWidth;
    GLsizei stride;
};

struct GLWRTexture2DDesc {
    GLsizei width;
    GLsizei height;
    GLenum textureFormat;
    GLenum pixelFormat;
    GLenum dataType;
};

struct GLWRSamplerDesc {
    GLWRFilter filter;
    GLWRTextureCoordinatesMode coordinateS;
    GLWRTextureCoordinatesMode coordinateT;
    GLWRTextureCoordinatesMode coordinateR;
};

struct GLWRRasterizerDesc {
    GLWRFillMode fillMode;
    GLWRCullMode cullMode;
};

struct GLWRBlendDesc {
    bool enable;
    GLWRBlend srcRGB;
    GLWRBlend dstRGB;
    GLWRBlendEq eqRGB;
    GLWRBlend srcAlpha;
    GLWRBlend dstAlpha;
    GLWRBlendEq eqAlpha;
};

struct GLWRResourceData {
    void const* mem;
};

template <typename T>
class GLWRPtr
{
    T* m_ptr;

public:
    GLWRPtr()
        : m_ptr(nullptr)
    {
    }
    ~GLWRPtr()
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

class _GLWRState
{
    using _Op = std::function<void(void)>;
    std::vector<_Op> m_ops;

public:
    void Add(_Op op);
    void Execute() const;
};

using GLWRRasterizerState = _GLWRState;
using GLWRBlendState = _GLWRState;

struct GLWRViewport {
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
        GLuint pipeline;
        GLuint vert;
        GLuint frag;
        GLuint layout;
        GLWRPtr<GLWRInputLayout> inputLayout;
        GLWRPtr<GLWRBuffer> buffer;
        GLenum screenTexture;
        GLenum screenSampler;
        GLenum targetFrame;
        GLWRPtr<GLWRRasterizerState> state;
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
    ~Graphics();
    void CreateRenderTarget(int width, int height, GLWRRenderTarget** ppRenderTarget);
    void CreateInputLayout(GLWRInputElementDesc const* inputElementDesc, unsigned int numElements,
                           GLuint const programWithInputSignature, GLWRInputLayout** ppInputLayout);
    void CreateBuffer(GLWRBufferDesc const* pDesc, GLWRResourceData const* initialData, GLWRBuffer** ppBuffer);
    void CreateTexture2D(GLWRTexture2DDesc const* pDesc, GLWRResourceData const* pInitialData,
                         GLWRTexture2D** ppTexture2D);
    void CreateSampler(GLWRSamplerDesc const* pDesc, GLWRSampler** ppSamplerState);
    void CreateShaderProgram(GLuint& program);
    void CreateProgramPipeline(GLuint& pipeline);
    void CreateSeparableShaderProgram(GLuint& program, GLWRShaderStage stage, std::string const& filename);
    void CreateRasterizerState(GLWRRasterizerDesc const& desc, GLWRRasterizerState** ppState);
    void CreateBlendState(GLWRBlendDesc const& desc, GLWRBlendState** ppState);
    void AttachShaderStage(GLuint const program, GLWRShaderStage stage, std::string const& filename);
    void LinkShaderProgram(GLuint const program);
    void SetPrimitive(GLenum primitive);
    void SetVertexBuffers(GLuint first, int numBuffers, GLWRBuffer* const* buffers, GLsizei const* strides,
                          GLintptr const* offsets);
    void SetRenderTarget(GLWRRenderTarget* target);
    void SetInputLayout(GLWRInputLayout* pInputLayout);
    void SetIndexBuffer(GLenum elementDataType, GLvoid const* offsetOfFirstIndex, GLWRBuffer const* pBuffer);
    void SetTexture2D(unsigned int startUnit, unsigned int numTextures, GLWRTexture2D* const* ppTexture2D);
    void SetSamplers(unsigned int startUnit, unsigned int numSamplers, GLWRSampler* const* ppSamplers);
    void SetShaderProgram(GLuint program);
    void SetProgramPipeline(GLuint pipeline);
    void SetProgramPipelineStages(GLuint pipeline, GLbitfield stages, GLuint program);
    void SetRasterizerState(GLWRRasterizerState const* state);
    void SetBlendState(GLWRBlendState const* state);
    void SetViewports(unsigned int numViewports, GLWRViewport* viewports);
    void SetUniformBuffer(GLuint const bindingIndex, GLWRBuffer const* pBuffer);

    void DeleteShaderProgram(GLuint& program);
    void DeleteProgramPipeline(GLuint& pipeline);
    void ClearRenderTarget(GLWRRenderTarget* target, float const color[4]) const;

    glm::mat4 GetViewProjectionMatrix() const;
    glm::vec3 GetCameraPosition() const;
    Camera& GetCamera();

    void UpdateBuffer(GLenum target, GLintptr offset, GLsizei byteWidth, GLWRResourceData const* data,
                      GLWRBuffer* pBuffer);

    void Draw(GLsizei vertexCount);
    void DrawIndexed(GLsizei indexCount);
    void DrawInstanced(GLsizei vertexCountPerInstance, GLsizei instanceCount);
    void Present();

private:
    int UniformLocation(std::string const& uniformName) const;
    bool IsShaderCompiled(GLuint const shaderObject);
    void CheckProgramStatus(GLuint const programObject);
    std::string SlurpShaderSource(std::string const& filename) const;

    struct Enum {
        static GLAttribFormat Resolve(GLWRFormat const format);
    };
};

#endif
