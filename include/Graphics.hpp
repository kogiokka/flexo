#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

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
        GLuint program;
        GLenum primitive;
        GLenum format;
        GLsizei count;
        GLsizei stride;
        const GLvoid* offset;
    };

    Context ctx_;

public:
    void ClearBuffer(float red, float green, float blue) const;
    void CreateBuffer(GLuint& buffer, BufferDesc const& desc, BufferData const& data);
    void CreateTexture2D(GLuint& texture, Texture2dDesc const& desc, BufferData const& data);
    void CreateShaderProgram(GLuint& program);
    void AttachShaderStage(GLuint const program, ShaderStage stage, std::string const& filepath);
    void LinkShaderProgram(GLuint const program);
    void SetPrimitive(GLenum primitive);
    void SetVertexBuffers(int startAttrib, int numBuffers, GLuint* buffers, GLintptr* offsets);
    void SetIndexBuffer(GLuint buffer, GLenum format, const GLvoid* offset);
    void SetTexture(GLenum target, GLuint texture);
    void SetShaderProgram(GLuint program);
    void SetUniformBuffer(GLuint const uniform, GLuint const bindingIndex);

    template <typename T>
    void UpdateUniformBuffer(GLuint const uniform, T const& uniformBlock);

    template <typename T, int D1, int D2 = 1>
    void SetUniform(std::string const& name, T* values);

    void Draw(GLsizei vertexCount);
    void DrawIndexed(GLsizei indexCount);
    void DrawInstanced(GLsizei vertexCountPerInstance, GLsizei instanceCount);

private:
    int UniformLocation(std::string const& uniformName) const;
    bool IsShaderCompiled(GLuint const shaderObject);
};

template <typename T>
void Graphics::UpdateUniformBuffer(GLuint const uniform, T const& uniformBlock)
{
    glBindBuffer(GL_UNIFORM_BUFFER, uniform);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &uniformBlock);
}

#endif
