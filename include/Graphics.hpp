#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

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

struct InputElementDesc {
    GLchar const* name;
    InputFormat format;
    InputClassification inputSlotClass;
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

struct BufferData {
    const void* mem;
};

struct LightUniform {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffusion;
    glm::vec3 specular;
};

struct MaterialUniform {
    glm::vec3 ambient;
    glm::vec3 diffusion;
    glm::vec3 specular;
    float shininess;
};

enum ShaderStage : GLenum {
    Vert = GL_VERTEX_SHADER,
    Tesc = GL_TESS_CONTROL_SHADER,
    Tese = GL_TESS_EVALUATION_SHADER,
    Geom = GL_GEOMETRY_SHADER,
    Frag = GL_FRAGMENT_SHADER,
    Comp = GL_COMPUTE_SHADER,
};

class Graphics
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
    Graphics(int width, int height);
    void CreateInputLayout(GLuint& layout, InputElementDesc const* inputElementDesc, int const numElements);
    void CreateBuffer(GLuint& buffer, BufferDesc const& desc, BufferData const& data);
    void CreateTexture2D(GLuint& texture, GLuint const unit, Texture2dDesc const& desc, BufferData const& data);
    void CreateShaderProgram(GLuint& program);
    void CreateProgramPipeline(GLuint& pipeline);
    void CreateSeparableShaderProgram(GLuint& program, ShaderStage stage, std::string const& filename);
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
    GLAttribFormat ExtractGLAttribFormat(InputFormat format) const;
};

#endif
