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

struct RasterizerDesc {
    FillMode fillMode;
    CullMode cullMode;
};

struct ResourceData {
    void const* mem;
};

class RasterizerState
{
    std::vector<std::function<void(void)>> m_ops;

public:
    void Append(std::function<void(void)> const& op)
    {
        m_ops.push_back(op);
    }
    void Operate() const
    {
        for (auto const& op : m_ops) {
            op();
        }
    }
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
    void CreateShaderProgram(GLuint& program);
    void CreateProgramPipeline(GLuint& pipeline);
    void CreateSeparableShaderProgram(GLuint& program, ShaderStage stage, std::string const& filename);
    void CreateRasterizerState(RasterizerDesc const& desc, RasterizerState** ppState);
    void AttachShaderStage(GLuint const program, ShaderStage stage, std::string const& filename);
    void LinkShaderProgram(GLuint const program);
    void SetPrimitive(GLenum primitive);
    void SetVertexBuffers(GLuint first, int numBuffers, GLuint const* buffers, GLintptr const* offsets,
                          GLsizei const* strides);
    void SetInputLayout(GLuint const layout);
    void SetIndexBuffer(GLuint buffer, GLenum elementDataType, GLvoid const* offsetOfFirstIndex);
    void SetTexture(GLenum target, GLuint texture, GLuint unit);
    void SetShaderProgram(GLuint program);
    void SetProgramPipeline(GLuint pipeline);
    void SetProgramPipelineStages(GLuint pipeline, GLbitfield stages, GLuint program);
    void SetRasterizerState(RasterizerState const* state);
    void SetUniformBuffer(GLuint const uniform, GLuint const bindingIndex);

    void DeleteInputLayout(GLuint& layout);
    void DeleteBuffer(GLuint& buffer);
    void DeleteTexture(GLuint& texture);
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
