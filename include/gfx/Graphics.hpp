#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <cstdint>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Attachable.hpp"
#include "Camera.hpp"
#include "gfx/glwr/GLWRPtr.hpp"
#include "gfx/glwr/IGLWRBuffer.hpp"
#include "gfx/glwr/IGLWRDepthStencilView.hpp"
#include "gfx/glwr/IGLWRFragmentShader.hpp"
#include "gfx/glwr/IGLWRInputLayout.hpp"
#include "gfx/glwr/IGLWRRenderBuffer.hpp"
#include "gfx/glwr/IGLWRRenderTargetView.hpp"
#include "gfx/glwr/IGLWRResource.hpp"
#include "gfx/glwr/IGLWRSampler.hpp"
#include "gfx/glwr/IGLWRShaderResourceView.hpp"
#include "gfx/glwr/IGLWRState.hpp"
#include "gfx/glwr/IGLWRTexture2D.hpp"
#include "gfx/glwr/IGLWRVertexShader.hpp"

#define STD140_ALIGN alignas(sizeof(float) * 4)

typedef enum {
    GLWRInputClassification_PerVertex,
    GLWRInputClassification_PerInstance,
} GLWRInputClassification;

typedef enum {
    GLWRFormat_Float2,
    GLWRFormat_Float3,
    GLWRFormat_Uint,
    GLWRFormat_Uint2,
    GLWRFormat_Uint3,
    GLWRFormat_Uint2Norm,
    GLWRFormat_Uint3Norm,
} GLWRFormat;

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

typedef enum {
    GLWRClearFlag_Depth = GL_DEPTH_BUFFER_BIT,
    GLWRClearFlag_Stencil = GL_STENCIL_BUFFER_BIT,
} GLWRClearFlag;

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
    GLenum internalFormat;
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

struct GLWRShaderResourceViewDesc {
    GLenum format;
    GLenum target;
};

struct GLWRRenderTargetViewDesc {
    GLenum internalFormat;
    GLenum dimensions;
};

struct GLWRDepthStencilViewDesc {
    GLenum internalFormat;
    GLenum dimensions;
};

struct GLWRRenderBufferDesc {
    GLenum internalFormat;
    GLsizei width;
    GLsizei height;
};

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
        GLenum indexBufferFormat;
        GLubyte* offsetOfFirstIndex;
        GLuint pipeline;
        GLWRPtr<IGLWRVertexShader> vert;
        GLWRPtr<IGLWRFragmentShader> frag;
        GLuint layout;
        GLWRPtr<IGLWRInputLayout> inputLayout;
        GLWRPtr<IGLWRBuffer> buffer;
        GLenum screenTexture;
        GLenum screenSampler;
        GLenum frame;
        GLWRPtr<IGLWRRasterizerState> state;
        std::vector<GLWRViewport> viewports;
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
    void CreateRenderTargetView(IGLWRResource* pResource, GLWRRenderTargetViewDesc const* pDesc,
                                IGLWRRenderTargetView** ppRenderTargetView);
    void CreateDepthStencilView(IGLWRResource* pResource, GLWRDepthStencilViewDesc const* pDesc,
                                IGLWRDepthStencilView** ppDepthStencilView);
    void CreateInputLayout(GLWRInputElementDesc const* inputElementDesc, unsigned int numElements,
                           IGLWRVertexShader const* pProgramWithInputSignature, IGLWRInputLayout** ppInputLayout);
    void CreateVertexShader(char const* source, IGLWRVertexShader** ppVertexShader);
    void CreateFragmentShader(char const* source, IGLWRFragmentShader** ppFragmentShader);
    void CreateBuffer(GLWRBufferDesc const* pDesc, GLWRResourceData const* initialData, IGLWRBuffer** ppBuffer);
    void CreateRenderBuffer(GLWRRenderBufferDesc const* pDesc, IGLWRRenderBuffer** ppRenderBuffer);
    void CreateTexture2D(GLWRTexture2DDesc const* pDesc, GLWRResourceData const* pInitialData,
                         IGLWRTexture2D** ppTexture2D);
    void CreateShaderResourceView(IGLWRResource* pResource, GLWRShaderResourceViewDesc const* pDesc,
                                  IGLWRShaderResourceView** ppResourceView);
    void CreateSampler(GLWRSamplerDesc const* pDesc, IGLWRSampler** ppSamplerState);
    void CreateRasterizerState(GLWRRasterizerDesc const* pDesc, IGLWRRasterizerState** ppState);
    void CreateBlendState(GLWRBlendDesc const* pDesc, IGLWRBlendState** ppState);
    void SetPrimitive(GLenum primitive);
    void SetVertexBuffers(GLuint first, int numBuffers, IGLWRBuffer* const* buffers, GLsizei const* strides,
                          GLintptr const* offsets);
    void SetRenderTargets(unsigned int numViews, IGLWRRenderTargetView* const* pRenderTargetView,
                          IGLWRDepthStencilView* ppDepthStencilView);
    void SetInputLayout(IGLWRInputLayout* pInputLayout);
    void SetVertexShader(IGLWRVertexShader* ppVertexShader);
    void SetFragmentShader(IGLWRFragmentShader* pFragmentShader);
    void SetIndexBuffer(IGLWRBuffer const* pBuffer, GLWRFormat format, unsigned int offset);
    void SetShaderResources(unsigned int startUnit, unsigned int numTextures,
                            IGLWRShaderResourceView* const* ppResourceViews);
    void SetSamplers(unsigned int startUnit, unsigned int numSamplers, IGLWRSampler* const* ppSamplers);
    void SetShaderProgram(GLuint program);
    void SetRasterizerState(IGLWRRasterizerState const* state);
    void SetBlendState(IGLWRBlendState const* state);
    void SetViewports(unsigned int numViewports, GLWRViewport* viewports);
    void SetUniformBuffer(GLuint const bindingIndex, IGLWRBuffer const* pBuffer);

    void ClearRenderTargetView(IGLWRRenderTargetView* pRenderTargetView, float const color[4]) const;
    void ClearDepthStencilView(IGLWRDepthStencilView* pDepthStencilView, GLWRClearFlag flags, float depth = 1.0f,
                               uint8_t stencil = 0) const;
    void GenerateMips(IGLWRShaderResourceView* pShaderResourceView);

    glm::mat4 GetViewProjectionMatrix() const;
    glm::vec3 GetCameraPosition() const;
    Camera& GetCamera();

    void UpdateBuffer(GLenum target, GLintptr offset, GLsizei byteWidth, GLWRResourceData const* data,
                      IGLWRBuffer* pBuffer);

    void Draw(GLsizei vertexCount);
    void DrawIndexed(GLsizei indexCount);
    void DrawInstanced(GLsizei vertexCountPerInstance, GLsizei instanceCount);
    void Present();

    static std::string SlurpShaderSource(std::string const& filename);
    static void CreateShaderResourceViewFromFile(Graphics* pContext, char const* filename,
                                                 IGLWRShaderResourceView** ppResourceView);

private:
    void AttachShaderStage(GLuint const program, GLenum stage, char const* source);
    int UniformLocation(std::string const& uniformName) const;
    bool IsShaderCompiled(GLuint const shaderObject);
    void LinkShaderProgram(GLuint const program);
    void CheckProgramStatus(GLuint const programObject);

    struct Enum {
        static GLAttribFormat Resolve(GLWRFormat const format);
    };
};

#endif
