#include <filesystem>
#include <fstream>

#include "Graphics.hpp"
#include "common/Logger.hpp"

void Graphics::ClearBuffer(float red, float green, float blue) const
{
    glClearColor(red, green, blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics::CreateBuffer(GLuint& buffer, BufferDesc const& desc, BufferData const& data)
{
    glGenBuffers(1, &buffer);
    glBindBuffer(desc.target, buffer);
    glBufferData(desc.target, desc.byteWidth, data.mem, desc.usage);
    ctx_.stride = desc.stride;
}

void Graphics::CreateTexture2D(GLuint& texture, const Texture2dDesc& desc, const BufferData& data)
{
    constexpr GLint BORDER = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, desc.textureFormat, desc.width, desc.height, BORDER, desc.pixelFormat, desc.dataType,
                 data.mem);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Graphics::CreateShaderProgram(GLuint& program)
{
    program = glCreateProgram();
}

void Graphics::AttachShaderStage(GLuint const program, ShaderStage stage, std::string const& filepath)
{
    using namespace std;

    ifstream file;
    string source;

    file.open(filepath.data());
    if (file.fail()) {
        Logger::error("Failed to open shader file: %s", filepath.c_str());
    }
    source.resize(filesystem::file_size(filepath));
    file.read(source.data(), source.size());
    file.close();

    char const* const shaderSourceArray[1] = { source.c_str() };

    GLuint shaderObject = glCreateShader(stage);
    glShaderSource(shaderObject, 1, shaderSourceArray, nullptr);
    glCompileShader(shaderObject);
    if (!IsShaderCompiled(shaderObject)) {
        Logger::error("Shader object compilation error: %s", filepath.c_str());
    }
    glAttachShader(program, shaderObject);
    glDeleteShader(shaderObject);
}

void Graphics::LinkShaderProgram(const GLuint program)
{
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_TRUE)
        return;

    GLint lenLog;
    std::string log;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &lenLog);
    log.resize(lenLog);
    glGetProgramInfoLog(program, lenLog, nullptr, log.data());
    glDeleteProgram(program);

    Logger::error("Shader program linking error: %s\n", log.data());
}

void Graphics::SetPrimitive(GLenum primitive)
{
    ctx_.primitive = primitive;
}

void Graphics::SetVertexBuffers(int startAttrib, int numBuffers, GLuint* buffers, GLintptr* offsets)
{
    for (int i = 0; i < numBuffers; i++) {
        int const attr = startAttrib + i;
        glBindBuffer(GL_ARRAY_BUFFER, attr);
        glBindVertexBuffer(attr, buffers[i], offsets[i], ctx_.stride);
    }
}

void Graphics::SetIndexBuffer(GLuint buffer, GLenum format, const GLvoid* offset)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    ctx_.format = format;
    ctx_.offset = offset;
}

void Graphics::SetTexture(GLenum target, GLuint texture)
{
    glBindTexture(target, texture);
}

void Graphics::SetShaderProgram(GLuint program)
{
    glUseProgram(program);
    ctx_.program = program;
}

void Graphics::Draw(GLsizei vertexCount)
{
    glDrawArrays(ctx_.primitive, 0, vertexCount);
}

void Graphics::DrawIndexed(GLsizei indexCount)
{
    glDrawElements(ctx_.primitive, indexCount, ctx_.count, ctx_.offset);
}

void Graphics::DrawInstanced(GLsizei vertexCountPerInstance, GLsizei instanceCount)
{
    glDrawArraysInstanced(ctx_.primitive, 0, vertexCountPerInstance, instanceCount);
}

int Graphics::UniformLocation(std::string const& uniformName) const
{
    char const* name = uniformName.data();

    int location = glGetUniformLocation(ctx_.program, name);
    if (location == -1) {
        Logger::error("Uniform %s does not exist.\n", name);
    }
    return location;
}

bool Graphics::IsShaderCompiled(GLuint const shaderObject)
{
    GLint success;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);

    if (success == GL_TRUE)
        return true;

    GLint lenLog;
    std::string log;

    glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &lenLog);
    log.resize(lenLog);
    glGetShaderInfoLog(shaderObject, lenLog, nullptr, log.data());
    glDeleteShader(shaderObject);

    std::fprintf(stderr, "%s\n", log.data());

    return false;
}

template <>
void Graphics::SetUniform<int, 1, 1>(std::string const& name, int* values)
{
    glUniform1iv(UniformLocation(name), 1, values);
}

template <>
void Graphics::SetUniform<float, 1, 1>(std::string const& name, float* values)
{
    glUniform1fv(UniformLocation(name), 1, values);
}

template <>
void Graphics::SetUniform<float, 3, 1>(std::string const& name, float* values)
{
    glUniform3fv(UniformLocation(name), 1, values);
}

template <>
void Graphics::SetUniform<float, 4, 4>(std::string const& name, float* values)
{
    glUniformMatrix4fv(UniformLocation(name), 1, GL_FALSE, values);
}
