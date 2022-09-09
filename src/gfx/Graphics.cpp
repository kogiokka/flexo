#include <cstdlib>
#include <fstream>

#include "Project.hpp"
#include "RenderTarget.hpp"
#include "common/Logger.hpp"
#include "gfx/Graphics.hpp"
#include "pane/SceneViewportPane.hpp"

void _GLWRState::Add(_Op op)
{
    m_ops.push_back(op);
}

void _GLWRState::Execute() const
{
    for (auto const& op : m_ops) {
        op();
    }
}

// Register factory: Graphics
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<Graphics> {
        auto const& viewport = SceneViewportPane::Get(project);
        wxSize const size = viewport.GetClientSize() * viewport.GetContentScaleFactor();
        return std::make_shared<Graphics>(size.x, size.y);
    }
};

Graphics& Graphics::Get(WatermarkingProject& project)
{
    return project.AttachedObjects::Get<Graphics>(factoryKey);
}

Graphics const& Graphics::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

Graphics::Graphics(int width, int height)
    : m_camera(width, height)
{
    m_ctx.framebuffer = 0;

    // Setup the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float quadVerts[] = { -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
                          -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f };

    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float2, 0, 0, GLWRInputClassification_PerVertex, 0 },
        { "textureCoord", GLWRFormat_Float2, 0, sizeof(float) * 2, GLWRInputClassification_PerVertex, 0 },
    };

    CreateProgramPipeline(m_ctx.pipeline);
    CreateSeparableShaderProgram(m_ctx.vert, GLWRShaderStage::GLWRShaderStage_Vert, "shader/Screen.vert");
    CreateSeparableShaderProgram(m_ctx.frag, GLWRShaderStage::GLWRShaderStage_Frag, "shader/Screen.frag");
    CreateInputLayout(m_ctx.layout, inputs.data(), inputs.size(), m_ctx.vert);

    GLWRBufferDesc bufferDesc;
    bufferDesc.target = GL_ARRAY_BUFFER;
    bufferDesc.usage = GL_STATIC_DRAW;
    bufferDesc.byteWidth = 4 * sizeof(float) * 6;
    bufferDesc.stride = 4 * sizeof(float);
    GLWRResourceData quadVertsData;
    quadVertsData.mem = quadVerts;
    CreateBuffer(m_ctx.buffer, bufferDesc, quadVertsData);

    GLWRRasterizerDesc rasDesc { GLWRFillMode::GLWRFillMode_Solid, GLWRCullMode::GLWRCullMode_Back };
    CreateRasterizerState(rasDesc, &m_ctx.state);
}

Graphics::~Graphics()
{
    DeleteProgramPipeline(m_ctx.pipeline);
    DeleteShaderProgram(m_ctx.vert);
    DeleteShaderProgram(m_ctx.frag);
    DeleteInputLayout(m_ctx.layout);
    DeleteBuffer(m_ctx.buffer);
}

void Graphics::CreateRenderTarget(int width, int height, GLWRRenderTarget** ppRenderTarget)
{
    *ppRenderTarget = new GLWRRenderTarget(width, height);
}

void Graphics::CreateInputLayout(GLuint& layout, GLWRInputElementDesc const* inputElementDesc, int const numElements,
                                 GLuint const programWithInputSignature)
{
    glGenVertexArrays(1, &layout);
    glBindVertexArray(layout);
    for (int i = 0; i < numElements; i++) {
        auto const& desc = inputElementDesc[i];
        GLint location = glGetAttribLocation(programWithInputSignature, desc.semanticName);
        if (location == -1) {
            Logger::error("Invalid semantic name: %s", desc.semanticName);
            return;
        }
        glEnableVertexAttribArray(location);
        auto const& [size, type, normalized] = Enum::Resolve(desc.format);
        glVertexAttribFormat(location, size, type, normalized, desc.byteOffset);
        glVertexAttribBinding(location, desc.inputSlot);
        glVertexBindingDivisor(desc.inputSlot, 0);
        if (desc.inputSlotClass == GLWRInputClassification_PerInstance) {
            glVertexBindingDivisor(desc.inputSlot, desc.instanceDataStepRate);
        }
    }
}

void Graphics::CreateBuffer(GLuint& buffer, GLWRBufferDesc const& desc, GLWRResourceData const& data)
{
    glGenBuffers(1, &buffer);
    glBindBuffer(desc.target, buffer);
    glBufferData(desc.target, desc.byteWidth, data.mem, desc.usage);
}

void Graphics::CreateTexture2D(GLuint& texture, GLuint const unit, GLWRTexture2DDesc const& desc,
                               GLWRResourceData const& data)
{
    constexpr GLint BORDER = 0;
    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, desc.textureFormat, desc.width, desc.height, BORDER, desc.pixelFormat, desc.dataType,
                 data.mem);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Graphics::CreateSampler(GLuint& sampler, GLWRSamplerDesc const& desc)
{
    glGenSamplers(1, &sampler);

    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, desc.coordinateS);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, desc.coordinateT);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, desc.coordinateR);

    switch (desc.filter) {
    case GLWRFilter_MinMagNearest_NoMip:
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case GLWRFilter_MinMagLinear_NoMip:
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case GLWRFilter_MinMagNearest_MipNearest:
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case GLWRFilter_MinMagNearest_MipLinear:
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case GLWRFilter_MinMagLinear_MipNearest:
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case GLWRFilter_MinMagLinear_MipLinear:
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    default:
        break;
    }
}

void Graphics::CreateShaderProgram(GLuint& program)
{
    program = glCreateProgram();
}

void Graphics::CreateProgramPipeline(GLuint& pipeline)
{
    glGenProgramPipelines(1, &pipeline);
}

void Graphics::CreateSeparableShaderProgram(GLuint& program, GLWRShaderStage stage, std::string const& filename)
{
    std::string const& source = SlurpShaderSource(filename);
    char const* const shaderSourceArray[1] = { source.c_str() };
    program = glCreateShaderProgramv(stage, 1, shaderSourceArray);

    CheckProgramStatus(program);
}

void Graphics::CreateRasterizerState(GLWRRasterizerDesc const& desc, GLWRRasterizerState** ppState)
{
    *ppState = new GLWRRasterizerState();
    GLWRRasterizerState* pState = *ppState;

    switch (desc.cullMode) {
    case GLWRCullMode::GLWRCullMode_Back:
        pState->Add(std::bind(glEnable, GL_CULL_FACE));
        pState->Add(std::bind(glCullFace, GL_BACK));
        break;
    case GLWRCullMode::GLWRCullMode_Front:
        pState->Add(std::bind(glEnable, GL_CULL_FACE));
        pState->Add(std::bind(glCullFace, GL_FRONT));
        break;
    case GLWRCullMode::GLWRCullMode_None:
        pState->Add(std::bind(glDisable, GL_CULL_FACE));
        break;
    };

    switch (desc.fillMode) {
    case GLWRFillMode::GLWRFillMode_WireFrame:
        pState->Add(std::bind(glPolygonMode, GL_FRONT_AND_BACK, GL_LINE));
        break;
    case GLWRFillMode::GLWRFillMode_Solid:
        pState->Add(std::bind(glPolygonMode, GL_FRONT_AND_BACK, GL_FILL));
        break;
    }
}

void Graphics::CreateBlendState(GLWRBlendDesc const& desc, GLWRBlendState** ppState)
{
    *ppState = new GLWRBlendState();
    GLWRBlendState* pState = *ppState;

    if (desc.enable) {
        pState->Add(std::bind(glEnable, GL_BLEND));
    } else {
        pState->Add(std::bind(glDisable, GL_BLEND));
    }

    pState->Add(std::bind(glBlendEquationSeparate, desc.eqRGB, desc.eqAlpha));
    pState->Add(std::bind(glBlendFuncSeparate, desc.srcRGB, desc.dstRGB, desc.srcAlpha, desc.dstAlpha));
}

void Graphics::SetViewports(unsigned int numViewports, GLWRViewport* viewports)
{
    float* viewportParams = static_cast<float*>(malloc(numViewports * 4 * sizeof(float)));
    double* depthRangeParams = static_cast<double*>(malloc(numViewports * 2 * sizeof(double)));

    for (unsigned int i = 0; i < numViewports; i++) {
        auto const& v = viewports[i];
        viewportParams[i * 4 + 0] = v.x;
        viewportParams[i * 4 + 1] = v.y;
        viewportParams[i * 4 + 2] = v.width;
        viewportParams[i * 4 + 3] = v.height;
        depthRangeParams[i * 2 + 0] = v.nearDepth;
        depthRangeParams[i * 2 + 1] = v.farDepth;
    }

    glViewportArrayv(0, numViewports, viewportParams);
    glDepthRangeArrayv(0, numViewports, depthRangeParams);

    free(viewportParams);
    free(depthRangeParams);
}

void Graphics::AttachShaderStage(GLuint const program, GLWRShaderStage stage, std::string const& filename)
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

void Graphics::SetRenderTarget(GLWRRenderTarget* target)
{
    glBindFramebuffer(GL_FRAMEBUFFER, target->GetFrame());
    m_ctx.framebuffer = target->GetFrame();
    m_ctx.screenTexture = target->GetTexture();
}

void Graphics::SetInputLayout(GLuint const layout)
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
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(target, texture);
}

void Graphics::SetSampler(GLuint unit, GLuint sampler)
{
    glBindSampler(unit, sampler);
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

void Graphics::SetRasterizerState(GLWRRasterizerState const* state)
{
    state->Execute();
}

void Graphics::SetBlendState(GLWRBlendState const* state)
{
    state->Execute();
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

void Graphics::Present()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    SetPrimitive(GL_TRIANGLES);
    GLintptr offset = 0;
    GLsizei stride = 4 * sizeof(float);
    SetVertexBuffers(0, 1, &m_ctx.buffer, &offset, &stride);

    SetProgramPipeline(m_ctx.pipeline);
    SetProgramPipelineStages(m_ctx.pipeline, GL_VERTEX_SHADER_BIT, m_ctx.vert);
    SetProgramPipelineStages(m_ctx.pipeline, GL_FRAGMENT_SHADER_BIT, m_ctx.frag);

    SetInputLayout(m_ctx.layout);

    // FIXME It seems to collide with LatticeFace texture when the unit is set to 0.
    SetTexture(GL_TEXTURE_2D, m_ctx.screenTexture, 20);
    SetRasterizerState(m_ctx.state.Get());

    glDisable(GL_DEPTH_TEST);
    Draw(6);
}

void Graphics::SetUniformBuffer(GLuint const uniform, GLuint const bindingIndex)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, uniform);
}

void Graphics::DeleteInputLayout(GLuint& layout)
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

void Graphics::DeleteSampler(GLuint& sampler)
{
    glDeleteSamplers(1, &sampler);
}

void Graphics::DeleteShaderProgram(GLuint& program)
{
    glDeleteProgram(program);
}

void Graphics::DeleteProgramPipeline(GLuint& pipeline)
{
    glDeleteProgramPipelines(1, &pipeline);
}

void Graphics::ClearRenderTarget(GLWRRenderTarget* target, float const color[4]) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, target->GetFrame());
    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ctx.framebuffer);
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
    std::fstream file;
    std::string source;
    std::size_t size;

    file.open(filename, std::fstream::in | std::fstream::binary);
    if (file.fail()) {
        Logger::error("Failed to open shader file: %s", filename.c_str());
    }

    file.seekg(0, std::fstream::end);
    size = file.tellg();
    file.seekg(0);

    source.resize(size);
    file.read(source.data(), size);

    file.close();

    return source;
}

Graphics::GLAttribFormat Graphics::Enum::Resolve(GLWRFormat const format)
{
    switch (format) {
    case GLWRFormat_Float2:
        return GLAttribFormat { 2, GL_FLOAT, GL_FALSE };
        break;
    case GLWRFormat_Float3:
        return GLAttribFormat { 3, GL_FLOAT, GL_FALSE };
        break;
    case GLWRFormat_Uint2:
        return GLAttribFormat { 2, GL_UNSIGNED_INT, GL_FALSE };
        break;
    case GLWRFormat_Uint3:
        return GLAttribFormat { 3, GL_UNSIGNED_INT, GL_FALSE };
        break;
    case GLWRFormat_Uint2Norm:
        return GLAttribFormat { 2, GL_UNSIGNED_INT, GL_TRUE };
        break;
    case GLWRFormat_Uint3Norm:
        return GLAttribFormat { 3, GL_UNSIGNED_INT, GL_TRUE };
        break;
    }
}
