#include <cstdlib>
#include <fstream>

#include "Project.hpp"
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
    m_ctx.targetFrame = 0;

    // Setup the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_ctx.targetFrame);

    float quadVerts[] = { -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
                          -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f };

    std::vector<GLWRInputElementDesc> inputs = {
        { "position", GLWRFormat_Float2, 0, 0, GLWRInputClassification_PerVertex, 0 },
        { "textureCoord", GLWRFormat_Float2, 0, sizeof(float) * 2, GLWRInputClassification_PerVertex, 0 },
    };

    glUseProgram(0);
    glGenProgramPipelines(1, &m_ctx.pipeline);
    glBindProgramPipeline(m_ctx.pipeline);

    std::string vertSource = SlurpShaderSource("shader/Screen.vert");
    std::string fragSource = SlurpShaderSource("shader/Screen.frag");
    CreateVertexShader(vertSource.data(), &m_ctx.vert);
    CreateFragmentShader(fragSource.data(), &m_ctx.frag);
    CreateInputLayout(inputs.data(), inputs.size(), m_ctx.vert.Get(), &m_ctx.inputLayout);

    GLWRBufferDesc bufferDesc;
    bufferDesc.target = GL_ARRAY_BUFFER;
    bufferDesc.usage = GL_STATIC_DRAW;
    bufferDesc.byteWidth = 4 * sizeof(float) * 6;
    bufferDesc.stride = 4 * sizeof(float);
    GLWRResourceData quadVertsData;
    quadVertsData.mem = quadVerts;
    CreateBuffer(&bufferDesc, &quadVertsData, &m_ctx.buffer);

    GLWRRasterizerDesc rasDesc { GLWRFillMode::GLWRFillMode_Solid, GLWRCullMode::GLWRCullMode_Back };
    CreateRasterizerState(&rasDesc, &m_ctx.state);
}

Graphics::~Graphics()
{
    glDeleteProgramPipelines(1, &m_ctx.pipeline);
}

void Graphics::CreateRenderTarget(int width, int height, GLWRRenderTarget** ppRenderTarget)
{
    *ppRenderTarget = new GLWRRenderTarget(width, height);
}

void Graphics::CreateInputLayout(GLWRInputElementDesc const* inputElementDesc, unsigned int numElements,
                                 GLWRVertexShader const* pProgramWithInputSignature, GLWRInputLayout** ppInputLayout)
{
    *ppInputLayout = new GLWRInputLayout();

    SetInputLayout(*ppInputLayout);
    for (unsigned int i = 0; i < numElements; i++) {
        auto const& desc = inputElementDesc[i];
        GLint location = glGetAttribLocation(pProgramWithInputSignature->m_id, desc.semanticName);
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
    SetInputLayout(m_ctx.inputLayout.Get());
}

void Graphics::CreateVertexShader(char const* source, GLWRVertexShader** ppVertexShader)
{
    *ppVertexShader = new GLWRVertexShader();

    GLuint const program = (*ppVertexShader)->m_id;
    AttachShaderStage(program, GL_VERTEX_SHADER, source);
    CheckProgramStatus(program);
}

void Graphics::CreateFragmentShader(char const* source, GLWRFragmentShader** ppFragmentShader)
{
    *ppFragmentShader = new GLWRFragmentShader();

    GLuint const program = (*ppFragmentShader)->m_id;
    AttachShaderStage(program, GL_FRAGMENT_SHADER, source);
    CheckProgramStatus(program);
}

void Graphics::CreateBuffer(GLWRBufferDesc const* pDesc, GLWRResourceData const* initialData, GLWRBuffer** ppBuffer)
{
    *ppBuffer = new GLWRBuffer();
    GLWRBuffer& buffer = **ppBuffer;

    glBindBuffer(pDesc->target, buffer.m_id);
    glBufferData(pDesc->target, pDesc->byteWidth, initialData->mem, pDesc->usage);
}

void Graphics::CreateTexture2D(GLWRTexture2DDesc const* pDesc, GLWRResourceData const* pInitialData,
                               GLWRTexture2D** ppTexture2D)
{
    *ppTexture2D = new GLWRTexture2D();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, (*ppTexture2D)->m_id); // Bind the new texture to the context so we can modify it.

    constexpr GLint BORDER = 0;
    glTexImage2D(GL_TEXTURE_2D, 0, pDesc->textureFormat, pDesc->width, pDesc->height, BORDER, pDesc->pixelFormat,
                 pDesc->dataType, pInitialData->mem);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Graphics::CreateSampler(GLWRSamplerDesc const* pDesc, GLWRSampler** ppSampler)
{
    *ppSampler = new GLWRSampler();
    GLuint sampler = (*ppSampler)->m_id;

    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, pDesc->coordinateS);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, pDesc->coordinateT);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, pDesc->coordinateR);

    switch (pDesc->filter) {
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

void Graphics::CreateRasterizerState(GLWRRasterizerDesc const* pDesc, GLWRRasterizerState** ppState)
{
    *ppState = new GLWRRasterizerState();
    GLWRRasterizerState* pState = *ppState;

    switch (pDesc->cullMode) {
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

    switch (pDesc->fillMode) {
    case GLWRFillMode::GLWRFillMode_WireFrame:
        pState->Add(std::bind(glPolygonMode, GL_FRONT_AND_BACK, GL_LINE));
        break;
    case GLWRFillMode::GLWRFillMode_Solid:
        pState->Add(std::bind(glPolygonMode, GL_FRONT_AND_BACK, GL_FILL));
        break;
    }
}

void Graphics::CreateBlendState(GLWRBlendDesc const* pDesc, GLWRBlendState** ppState)
{
    *ppState = new GLWRBlendState();
    GLWRBlendState* pState = *ppState;

    if (pDesc->enable) {
        pState->Add(std::bind(glEnable, GL_BLEND));
    } else {
        pState->Add(std::bind(glDisable, GL_BLEND));
    }

    pState->Add(std::bind(glBlendEquationSeparate, pDesc->eqRGB, pDesc->eqAlpha));
    pState->Add(std::bind(glBlendFuncSeparate, pDesc->srcRGB, pDesc->dstRGB, pDesc->srcAlpha, pDesc->dstAlpha));
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

void Graphics::AttachShaderStage(GLuint const program, GLenum stage, char const* source)
{
    char const* const shaderSourceArray[1] = { source };

    GLuint shaderObject = glCreateShader(stage);
    glShaderSource(shaderObject, 1, shaderSourceArray, nullptr);
    glCompileShader(shaderObject);
    if (!IsShaderCompiled(shaderObject)) {
        Logger::error("Shader object compilation error: %s", source);
    }
    glAttachShader(program, shaderObject);
    glDeleteShader(shaderObject);

    LinkShaderProgram(program);
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
    glBindFramebuffer(GL_FRAMEBUFFER, target->m_frame);

    m_ctx.screenTexture = target->m_texture->m_id;
    m_ctx.screenSampler = target->m_sampler->m_id;
    m_ctx.targetFrame = target->m_frame;
}

void Graphics::SetInputLayout(GLWRInputLayout* pInputLayout)
{
    glBindVertexArray(pInputLayout->m_id);
}

void Graphics::SetVertexShader(GLWRVertexShader* ppVertexShader)
{
    glUseProgramStages(m_ctx.pipeline, GL_VERTEX_SHADER_BIT, ppVertexShader->m_id);
}

void Graphics::SetFragmentShader(GLWRFragmentShader* ppFragmentShader)
{
    glUseProgramStages(m_ctx.pipeline, GL_FRAGMENT_SHADER_BIT, ppFragmentShader->m_id);
}

void Graphics::SetPrimitive(GLenum primitive)
{
    m_ctx.primitive = primitive;
}

void Graphics::SetVertexBuffers(GLuint first, int numBuffers, GLWRBuffer* const* buffers, GLsizei const* strides,
                                GLintptr const* offsets)
{
    for (int i = 0; i < numBuffers; i++) {
        glBindVertexBuffer(first + i, buffers[i]->m_id, offsets[i], strides[i]);
    }
}

void Graphics::SetIndexBuffer(GLWRBuffer const* pBuffer, GLWRFormat format, unsigned int offset)
{
    auto const attrFormat = Enum::Resolve(format);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pBuffer->m_id);

    m_ctx.indexBufferFormat = attrFormat.type;
    m_ctx.offsetOfFirstIndex = 0;
    m_ctx.offsetOfFirstIndex += offset;
}

void Graphics::SetTexture2D(unsigned int startUnit, unsigned int numTextures, GLWRTexture2D* const* ppTexture2D)
{
    for (unsigned int i = 0; i < numTextures; i++) {
        glActiveTexture(GL_TEXTURE0 + startUnit + i);
        glBindTexture(GL_TEXTURE_2D, ppTexture2D[i]->m_id);
    }
}

void Graphics::SetSamplers(unsigned int startUnit, unsigned int numSamplers, GLWRSampler* const* ppSamplers)
{
    for (unsigned int i = 0; i < numSamplers; i++) {
        glBindSampler(startUnit + i, ppSamplers[i]->m_id);
    }
}

void Graphics::SetShaderProgram(GLuint program)
{
    glUseProgram(program);
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
    glDrawElements(m_ctx.primitive, indexCount, m_ctx.indexBufferFormat, m_ctx.offsetOfFirstIndex);
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
    SetVertexBuffers(0, 1, &m_ctx.buffer, &stride, &offset);

    SetVertexShader(m_ctx.vert.Get());
    SetFragmentShader(m_ctx.frag.Get());

    SetInputLayout(m_ctx.inputLayout.Get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ctx.screenTexture);
    glBindSampler(0, m_ctx.screenSampler);
    SetRasterizerState(m_ctx.state.Get());

    glDisable(GL_DEPTH_TEST);
    Draw(6);

    glBindFramebuffer(GL_FRAMEBUFFER, m_ctx.targetFrame);
}

void Graphics::SetUniformBuffer(GLuint const bindingIndex, GLWRBuffer const* pBuffer)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, pBuffer->m_id);
}

void Graphics::ClearRenderTarget(GLWRRenderTarget* target, float const color[4]) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, target->m_frame);
    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ctx.targetFrame);
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

void Graphics::UpdateBuffer(GLenum target, GLintptr offset, GLsizei byteWidth, GLWRResourceData const* data,
                            GLWRBuffer* pBuffer)
{
    glBindBuffer(target, pBuffer->m_id);
    glBufferSubData(target, offset, byteWidth, data->mem);
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

std::string Graphics::SlurpShaderSource(std::string const& filename)
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
    case GLWRFormat_Uint:
        return GLAttribFormat { 1, GL_UNSIGNED_INT, GL_FALSE };
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
