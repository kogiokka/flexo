#include <filesystem>
#include <fstream>

#include "Graphics.hpp"
#include "common/Logger.hpp"

Graphics::Graphics(int width, int height)
    : m_camera(width, height)
{
}

void Graphics::CreateVertexLayout(GLuint& layout, AttributeDesc const* attrDescs, int const numAttrs)
{
    glGenVertexArrays(1, &layout);
    glBindVertexArray(layout);
    for (int i = 0; i < numAttrs; i++) {
        glEnableVertexAttribArray(i);
        glVertexAttribFormat(i, attrDescs[i].numValues, attrDescs[i].valueType, attrDescs[i].shouldNormalize, 0);

        glVertexBindingDivisor(i, 0);
        if (attrDescs[i].inputSlotClass == InputClassification::PerInstance) {
            glVertexBindingDivisor(i, 1);
        }
    }
}

void Graphics::CreateBuffer(GLuint& buffer, BufferDesc const& desc, BufferData const& data)
{
    glGenBuffers(1, &buffer);
    glBindBuffer(desc.target, buffer);
    glBufferData(desc.target, desc.byteWidth, data.mem, desc.usage);
}

void Graphics::CreateTexture2D(GLuint& texture, GLuint const unit, Texture2dDesc const& desc, BufferData const& data)
{
    constexpr GLint BORDER = 0;
    glGenTextures(1, &texture);

    glActiveTexture(unit);
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

void Graphics::CreateProgramPipeline(GLuint& pipeline)
{
    glGenProgramPipelines(1, &pipeline);
}

void Graphics::CreateSeparableShaderProgram(GLuint& program, ShaderStage stage, std::string const& filename)
{
    std::string const& source = SlurpShaderSource(filename);
    char const* const shaderSourceArray[1] = { source.c_str() };
    program = glCreateShaderProgramv(stage, 1, shaderSourceArray);

    CheckProgramStatus(program);
}

void Graphics::AttachShaderStage(GLuint const program, ShaderStage stage, std::string const& filename)
{
    std::string const& source = SlurpShaderSource(filename);
    char const* const shaderSourceArray[1] = { source.c_str() };

    GLuint shaderObject = glCreateShader(stage);
    glShaderSource(shaderObject, 1, shaderSourceArray, nullptr);
    glCompileShader(shaderObject);
    if (!IsShaderCompiled(shaderObject)) {
        Logger::error("Shader object compilation error: %s", filename.c_str());
    }
    glAttachShader(program, shaderObject);
    glDeleteShader(shaderObject);
}

void Graphics::LinkShaderProgram(const GLuint program)
{
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    CheckProgramStatus(program);
}

void Graphics::SetVertexLayout(GLuint const layout)
{
    glBindVertexArray(layout);
}

void Graphics::SetPrimitive(GLenum primitive)
{
    m_ctx.primitive = primitive;
}

void Graphics::SetVertexBuffers(GLuint first, int numBuffers, GLuint const* buffers, GLintptr const* offsets,
                                GLsizei const* strides)
{
    for (int i = 0; i < numBuffers; i++) {
        glBindVertexBuffer(first + i, buffers[i], offsets[i], strides[i]);
    }
}

void Graphics::SetIndexBuffer(GLuint buffer, GLenum elementDataType, const GLvoid* offsetOfFirstIndex)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    m_ctx.elementDataType = elementDataType;
    m_ctx.offsetOfFirstIndex = offsetOfFirstIndex;
}

void Graphics::SetTexture(GLenum target, GLuint texture, GLuint unit)
{
    glActiveTexture(unit);
    glBindTexture(target, texture);
}

void Graphics::SetShaderProgram(GLuint program)
{
    glUseProgram(program);
}

void Graphics::SetProgramPipeline(GLuint pipeline)
{
    glBindProgramPipeline(pipeline);
}

void Graphics::SetProgramPipelineStages(GLuint pipeline, GLbitfield stages, GLuint program)
{
    glUseProgram(0);
    glBindProgramPipeline(pipeline);
    glUseProgramStages(pipeline, stages, program);
}

void Graphics::Draw(GLsizei vertexCount)
{
    glDrawArrays(m_ctx.primitive, 0, vertexCount);
}

void Graphics::DrawIndexed(GLsizei indexCount)
{
    glDrawElements(m_ctx.primitive, indexCount, m_ctx.elementDataType, m_ctx.offsetOfFirstIndex);
}

void Graphics::DrawInstanced(GLsizei vertexCountPerInstance, GLsizei instanceCount)
{
    glDrawArraysInstanced(m_ctx.primitive, 0, vertexCountPerInstance, instanceCount);
}

void Graphics::SetUniformBuffer(GLuint const uniform, GLuint const bindingIndex)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, uniform);
}

void Graphics::ClearBuffer(float red, float green, float blue) const
{
    glClearColor(red, green, blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics::DeleteVertexLayout(GLuint& layout)
{
    glDeleteVertexArrays(1, &layout);
}

void Graphics::DeleteBuffer(GLuint& buffer)
{
    glDeleteBuffers(1, &buffer);
}

void Graphics::DeleteTexture(GLuint& texture)
{
    glDeleteTextures(1, &texture);
}

void Graphics::DeleteShaderProgram(GLuint& program)
{
    glDeleteProgram(program);
}

void Graphics::DeleteProgramPipeline(GLuint& pipeline)
{
    glDeleteProgramPipelines(1, &pipeline);
}

glm::mat4 Graphics::GetViewProjectionMatrix() const
{
    return m_camera.ViewProjectionMatrix();
}

glm::vec3 Graphics::GetCameraPosition() const
{
    return m_camera.position;
}

Camera& Graphics::GetCamera()
{
    return m_camera;
}

void Graphics::UpdateBuffer(GLuint const buffer, GLenum target, GLintptr offset, GLsizei byteWidth, void const* data)
{
    glBindBuffer(target, buffer);
    glBufferSubData(target, offset, byteWidth, data);
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

void Graphics::CheckProgramStatus(GLuint const program)
{
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_TRUE) {
        return;
    }

    GLint lenLog;
    std::string log;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &lenLog);
    log.resize(lenLog);
    glGetProgramInfoLog(program, lenLog, nullptr, log.data());
    glDeleteProgram(program);
    Logger::error("Shader program linking error: %s\n", log.data());
}

std::string Graphics::SlurpShaderSource(std::string const& filename) const
{
    std::ifstream file;
    std::string source;

    file.open(filename.data());
    if (file.fail()) {
        Logger::error("Failed to open shader file: %s", filename.c_str());
    }
    source.resize(std::filesystem::file_size(filename));
    file.read(source.data(), source.size());
    file.close();

    return source;
}
