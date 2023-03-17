#include <cstdlib>
#include <fstream>

#include <stb/image.h>

#include "Project.hpp"
#include "gfx/Graphics.hpp"
#include "log/Logger.h"

// Register factory: Graphics
static FlexoProject::AttachedObjects::RegisteredFactory const factoryKey { [](FlexoProject&) -> SharedPtr<Graphics> {
    return std::make_shared<Graphics>();
} };

Graphics& Graphics::Get(FlexoProject& project)
{
    return project.AttachedObjects::Get<Graphics>(factoryKey);
}

Graphics const& Graphics::Get(FlexoProject const& project)
{
    return Get(const_cast<FlexoProject&>(project));
}

Graphics::Graphics()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind the Default framebuffer

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
    bufferDesc.type = GLWRResourceType_Buffer;
    bufferDesc.usage = GL_STATIC_DRAW;
    bufferDesc.byteWidth = 4 * sizeof(float) * 6;
    bufferDesc.stride = 4 * sizeof(float);
    GLWRResourceData quadVertsData;
    quadVertsData.mem = quadVerts;
    CreateBuffer(&bufferDesc, &quadVertsData, &m_ctx.buffer);

    GLWRRasterizerDesc rasDesc { GLWRFillMode_Solid, GLWRCullMode_Back };
    CreateRasterizerState(&rasDesc, &m_ctx.state);

    glGenFramebuffers(1, &m_ctx.frame); // The framebuffer that will become our render target.
    glBindFramebuffer(GL_FRAMEBUFFER, m_ctx.frame);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
}

Graphics::~Graphics()
{
    glDeleteFramebuffers(1, &m_ctx.frame);
    glDeleteProgramPipelines(1, &m_ctx.pipeline);
}

void Graphics::CreateRenderTargetView(IGLWRResource* pResource, GLWRRenderTargetViewDesc const* pDesc,
                                      IGLWRRenderTargetView** ppRenderTargetView)
{
    *ppRenderTargetView = new IGLWRRenderTargetView();
    IGLWRRenderTargetView* self = *ppRenderTargetView;
    self->m_type = pDesc->type;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, self->m_clearFrame);

    // Bind attachments according to the type specified in the description struct.
    switch (pDesc->type) {
    case GLWRRenderTargetViewType_Texture2D:
        glTextureView(self->m_id, GL_TEXTURE_2D, pResource->m_id, pDesc->internalFormat, 0, 1, 0, 1);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->m_id, 0);
        break;
    case GLWRRenderTargetViewType_Texture2D_MS:
        glTextureView(self->m_id, GL_TEXTURE_2D_MULTISAMPLE, pResource->m_id, pDesc->internalFormat, 0, 1, 0, 1);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, self->m_id, 0);
        break;
    case GLWRRenderTargetViewType_RenderBuffer:
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, pResource->m_id);
        // Transferring the ownership.
        self->m_id = pResource->m_id;
        // The name zero is reserved by the GL and will be silently ignored by glDelete*.
        pResource->m_id = 0;
        break;
    default:
        break;
    };

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ctx.frame);
}

void Graphics::CreateDepthStencilView(IGLWRResource* pResource, GLWRDepthStencilViewDesc const* pDesc,
                                      IGLWRDepthStencilView** ppDepthStencilView)
{
    *ppDepthStencilView = new IGLWRDepthStencilView();
    IGLWRDepthStencilView* self = *ppDepthStencilView;
    self->m_type = pDesc->type;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, self->m_clearFrame);

    // Bind attachments according to the type specified in the description struct.
    switch (pDesc->type) {
    case GLWRDepthStencilViewType_Texture2D:
        glTextureView(self->m_id, GL_TEXTURE_2D, pResource->m_id, pDesc->internalFormat, 0, 1, 0, 1);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, self->m_id, 0);
        break;
    case GLWRDepthStencilViewType_Texture2D_MS:
        glTextureView(self->m_id, GL_TEXTURE_2D_MULTISAMPLE, pResource->m_id, pDesc->internalFormat, 0, 1, 0, 1);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, self->m_id,
                               0);
        break;
    case GLWRDepthStencilViewType_RenderBuffer:
    case GLWRDepthStencilViewType_RenderBuffer_MS:
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pResource->m_id);
        // Transferring the ownership.
        self->m_id = pResource->m_id;
        // The name zero is reserved by the GL and will be silently ignored by glDelete*.
        pResource->m_id = 0;
        break;
    default:
        break;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ctx.frame);
}

void Graphics::CreateInputLayout(GLWRInputElementDesc const* inputElementDesc, unsigned int numElements,
                                 IGLWRVertexShader const* pProgramWithInputSignature, IGLWRInputLayout** ppInputLayout)
{
    *ppInputLayout = new IGLWRInputLayout();
    IGLWRInputLayout* self = *ppInputLayout;

    glBindVertexArray(self->m_id);
    for (unsigned int i = 0; i < numElements; i++) {
        auto const& desc = inputElementDesc[i];
        GLint location = glGetAttribLocation(pProgramWithInputSignature->m_id, desc.semanticName);
        if (location == -1) {
            log_error("Invalid semantic name: %s", desc.semanticName);
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
    glBindVertexArray(m_ctx.inputLayout.Get()->m_id);
}

void Graphics::CreateVertexShader(char const* source, IGLWRVertexShader** ppVertexShader)
{
    *ppVertexShader = new IGLWRVertexShader();
    IGLWRVertexShader* self = *ppVertexShader;

    AttachShaderStage(self->m_id, GL_VERTEX_SHADER, source);
    CheckProgramStatus(self->m_id);
}

void Graphics::CreateFragmentShader(char const* source, IGLWRFragmentShader** ppFragmentShader)
{
    *ppFragmentShader = new IGLWRFragmentShader();
    IGLWRFragmentShader* self = *ppFragmentShader;

    AttachShaderStage(self->m_id, GL_FRAGMENT_SHADER, source);
    CheckProgramStatus(self->m_id);
}

void Graphics::CreateBuffer(GLWRBufferDesc const* pDesc, GLWRResourceData const* initialData, IGLWRBuffer** ppBuffer)
{
    *ppBuffer = new IGLWRBuffer();
    IGLWRBuffer* self = *ppBuffer;
    self->m_type = pDesc->type;

    switch (pDesc->type) {
    case GLWRResourceType_Buffer:
        glBindBuffer(GL_ARRAY_BUFFER, self->m_id);
        glBufferData(GL_ARRAY_BUFFER, pDesc->byteWidth, initialData->mem, pDesc->usage);
        break;
    case GLWRResourceType_IndexBuffer:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->m_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, pDesc->byteWidth, initialData->mem, pDesc->usage);
        break;
    case GLWRResourceType_UniformBuffer:
        glBindBuffer(GL_UNIFORM_BUFFER, self->m_id);
        glBufferData(GL_UNIFORM_BUFFER, pDesc->byteWidth, initialData->mem, pDesc->usage);
        break;
    default:
        log_error("Graphics::CreateBuffer currently does not support this resource type.");
        break;
    }
}

void Graphics::CreateTexture2D(GLWRTexture2DDesc const* pDesc, GLWRResourceData const* pInitialData,
                               IGLWRTexture2D** ppTexture2D)
{
    *ppTexture2D = new IGLWRTexture2D();
    IGLWRTexture2D* self = *ppTexture2D;
    self->m_type = GLWRResourceType_Texture2D;

    glActiveTexture(GL_TEXTURE0);
    if (pDesc->samples == 1) {
        glBindTexture(GL_TEXTURE_2D, self->m_id); // Bind the new texture to the context so we can modify it.
        glTexStorage2D(GL_TEXTURE_2D, 1, pDesc->internalFormat, pDesc->width, pDesc->height);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pDesc->width, pDesc->height, pDesc->pixelFormat, pDesc->dataType,
                        pInitialData->mem);
    } else if (pDesc->samples > 1) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,
                      self->m_id); // Bind the new texture to the context so we can modify it.
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, pDesc->samples, pDesc->internalFormat, pDesc->width,
                                  pDesc->height, GL_TRUE);
    }
}

void Graphics::CreateRenderBuffer(GLWRRenderBufferDesc const* pDesc, IGLWRRenderBuffer** ppRenderBuffer)
{
    *ppRenderBuffer = new IGLWRRenderBuffer();
    IGLWRRenderBuffer* self = *ppRenderBuffer;
    self->m_type = GLWRResourceType_RenderBuffer;

    glBindRenderbuffer(GL_RENDERBUFFER, self->m_id);
    if (pDesc->samples == 1) {
        glRenderbufferStorage(GL_RENDERBUFFER, pDesc->internalFormat, pDesc->width, pDesc->height);
    } else if (pDesc->samples >= 1) {
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, pDesc->samples, pDesc->internalFormat, pDesc->width,
                                         pDesc->height);
    }
}

void Graphics::CreateShaderResourceView(IGLWRResource* pResource, GLWRShaderResourceViewDesc const* pDesc,
                                        IGLWRShaderResourceView** ppResourceView)
{
    *ppResourceView = new IGLWRShaderResourceView();
    IGLWRShaderResourceView* self = *ppResourceView;
    self->m_type = pDesc->type;

    glTextureView(self->m_id, pDesc->type, pResource->m_id, pDesc->format, 0, 1, 0, 1);
}

void Graphics::CreateSampler(GLWRSamplerDesc const* pDesc, IGLWRSampler** ppSampler)
{
    *ppSampler = new IGLWRSampler();
    IGLWRSampler* self = *ppSampler;

    glSamplerParameteri(self->m_id, GL_TEXTURE_WRAP_S, pDesc->coordinateS);
    glSamplerParameteri(self->m_id, GL_TEXTURE_WRAP_T, pDesc->coordinateT);
    glSamplerParameteri(self->m_id, GL_TEXTURE_WRAP_R, pDesc->coordinateR);

    switch (pDesc->filter) {
    case GLWRFilter_MinMagNearest_NoMip:
        glSamplerParameteri(self->m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(self->m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case GLWRFilter_MinMagLinear_NoMip:
        glSamplerParameteri(self->m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(self->m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case GLWRFilter_MinMagNearest_MipNearest:
        glSamplerParameteri(self->m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glSamplerParameteri(self->m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case GLWRFilter_MinMagNearest_MipLinear:
        glSamplerParameteri(self->m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glSamplerParameteri(self->m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case GLWRFilter_MinMagLinear_MipNearest:
        glSamplerParameteri(self->m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glSamplerParameteri(self->m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case GLWRFilter_MinMagLinear_MipLinear:
        glSamplerParameteri(self->m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(self->m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    default:
        break;
    }
}

void Graphics::CreateRasterizerState(GLWRRasterizerDesc const* pDesc, IGLWRRasterizerState** ppState)
{
    *ppState = new IGLWRRasterizerState();
    IGLWRRasterizerState* self = *ppState;

    switch (pDesc->cullMode) {
    case GLWRCullMode_Back:
        self->Add(std::bind(glEnable, GL_CULL_FACE));
        self->Add(std::bind(glCullFace, GL_BACK));
        break;
    case GLWRCullMode_Front:
        self->Add(std::bind(glEnable, GL_CULL_FACE));
        self->Add(std::bind(glCullFace, GL_FRONT));
        break;
    case GLWRCullMode_None:
        self->Add(std::bind(glDisable, GL_CULL_FACE));
        break;
    };

    switch (pDesc->fillMode) {
    case GLWRFillMode_WireFrame:
        self->Add(std::bind(glPolygonMode, GL_FRONT_AND_BACK, GL_LINE));
        break;
    case GLWRFillMode_Solid:
        self->Add(std::bind(glPolygonMode, GL_FRONT_AND_BACK, GL_FILL));
        break;
    }
}

void Graphics::CreateBlendState(GLWRBlendDesc const* pDesc, IGLWRBlendState** ppState)
{
    *ppState = new IGLWRBlendState();
    IGLWRBlendState* self = *ppState;

    if (pDesc->enable) {
        self->Add(std::bind(glEnable, GL_BLEND));
    } else {
        self->Add(std::bind(glDisable, GL_BLEND));
    }

    self->Add(std::bind(glBlendEquationSeparate, pDesc->eqRGB, pDesc->eqAlpha));
    self->Add(std::bind(glBlendFuncSeparate, pDesc->srcRGB, pDesc->dstRGB, pDesc->srcAlpha, pDesc->dstAlpha));
}

void Graphics::SetViewports(unsigned int numViewports, GLWRViewport* viewports)
{
    float* viewportParams = static_cast<float*>(malloc(numViewports * 4 * sizeof(float)));
    double* depthRangeParams = static_cast<double*>(malloc(numViewports * 2 * sizeof(double)));

    m_ctx.viewports.clear();
    m_ctx.viewports.resize(numViewports);
    for (unsigned int i = 0; i < numViewports; i++) {
        auto const& v = viewports[i];
        viewportParams[i * 4 + 0] = v.x;
        viewportParams[i * 4 + 1] = v.y;
        viewportParams[i * 4 + 2] = v.width;
        viewportParams[i * 4 + 3] = v.height;
        depthRangeParams[i * 2 + 0] = v.nearDepth;
        depthRangeParams[i * 2 + 1] = v.farDepth;
        m_ctx.viewports[i] = v;
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
        log_error("Shader object compilation error: %s", source);
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

void Graphics::SetRenderTargets(unsigned int numViews, IGLWRRenderTargetView* const* pRenderTargetView,
                                IGLWRDepthStencilView* pDepthStencilView)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_ctx.frame);
    m_ctx.screenTexture = pRenderTargetView[0]->m_id;
    m_ctx.screenSampler = pRenderTargetView[0]->m_sampler;

    GLint const mipmapLevel = 0;
    GLenum* buffers = static_cast<GLenum*>(malloc(numViews * sizeof(GLenum)));

    // Attach color buffers
    for (unsigned int i = 0; i < numViews; i++) {
        GLenum const attachment = GL_COLOR_ATTACHMENT0 + i;
        buffers[i] = attachment;

        switch (pRenderTargetView[i]->m_type) {
        case GLWRRenderTargetViewType_Texture2D:
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, pRenderTargetView[i]->m_id, mipmapLevel);
            break;
        case GLWRRenderTargetViewType_Texture2D_MS:
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, pRenderTargetView[i]->m_id,
                                   mipmapLevel);
            break;
        case GLWRRenderTargetViewType_RenderBuffer:
        case GLWRRenderTargetViewType_RenderBuffer_MS:
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, pRenderTargetView[i]->m_id);
            break;
        default:
            break;
        }
    }
    glDrawBuffers(numViews, buffers);
    free(buffers);

    // Attach depth-stencil buffer
    switch (pDepthStencilView->m_type) {
    case GLWRDepthStencilViewType_Texture2D:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, pDepthStencilView->m_id,
                               mipmapLevel);
        break;
    case GLWRDepthStencilViewType_Texture2D_MS:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE,
                               pDepthStencilView->m_id, mipmapLevel);
        break;
    case GLWRDepthStencilViewType_RenderBuffer:
    case GLWRDepthStencilViewType_RenderBuffer_MS:
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                  pDepthStencilView->m_id);
        break;
    default:
        break;
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        log_error("Framebuffer failed to generate!");
    }
}

void Graphics::SetInputLayout(IGLWRInputLayout* pInputLayout)
{
    glBindVertexArray(pInputLayout->m_id);
}

void Graphics::SetVertexShader(IGLWRVertexShader* ppVertexShader)
{
    glUseProgramStages(m_ctx.pipeline, GL_VERTEX_SHADER_BIT, ppVertexShader->m_id);
}

void Graphics::SetFragmentShader(IGLWRFragmentShader* ppFragmentShader)
{
    glUseProgramStages(m_ctx.pipeline, GL_FRAGMENT_SHADER_BIT, ppFragmentShader->m_id);
}

void Graphics::SetPrimitive(GLenum primitive)
{
    m_ctx.primitive = primitive;
}

void Graphics::SetVertexBuffers(unsigned int startSlot, unsigned int numBuffers, IGLWRBuffer* const* ppVertexBuffers,
                                unsigned int const* pStrides, unsigned int const* pOffsets)
{
    for (unsigned int i = 0; i < numBuffers; i++) {
        GLintptr const offset = static_cast<GLintptr>(pOffsets[i]);
        GLsizei const stride = static_cast<GLsizei>(pStrides[i]);
        glBindVertexBuffer(startSlot + i, ppVertexBuffers[i]->m_id, offset, stride);
    }
}

void Graphics::SetUniformBuffers(unsigned int startSlot, unsigned int numBuffers, IGLWRBuffer* const* ppUniformBuffers)
{
    for (unsigned int i = 0; i < numBuffers; i++) {
        glBindBufferBase(GL_UNIFORM_BUFFER, startSlot + i, ppUniformBuffers[i]->m_id);
    }
}

void Graphics::SetIndexBuffer(IGLWRBuffer const* pBuffer, GLWRFormat format, unsigned int offset)
{
    auto const attrFormat = Enum::Resolve(format);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pBuffer->m_id);

    m_ctx.indexBufferFormat = attrFormat.type;
    m_ctx.offsetOfFirstIndex = 0;
    m_ctx.offsetOfFirstIndex += offset;
}

void Graphics::SetShaderResources(unsigned int startUnit, unsigned int numTextures,
                                  IGLWRShaderResourceView* const* ppResourceViews)
{
    for (unsigned int i = 0; i < numTextures; i++) {
        glActiveTexture(GL_TEXTURE0 + startUnit + i);
        glBindTexture(ppResourceViews[i]->m_type, ppResourceViews[i]->m_id);
    }
}

void Graphics::SetSamplers(unsigned int startUnit, unsigned int numSamplers, IGLWRSampler* const* ppSamplers)
{
    for (unsigned int i = 0; i < numSamplers; i++) {
        glBindSampler(startUnit + i, ppSamplers[i]->m_id);
    }
}

void Graphics::SetRasterizerState(IGLWRRasterizerState const* state)
{
    state->Execute();
}

void Graphics::SetBlendState(IGLWRBlendState const* state)
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
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // SetPrimitive(GL_TRIANGLES);
    // GLintptr offset = 0;
    // GLsizei stride = 4 * sizeof(float);
    // SetVertexBuffers(0, 1, &m_ctx.buffer, &stride, &offset);
    //
    // SetVertexShader(m_ctx.vert.Get());
    // SetFragmentShader(m_ctx.frag.Get());
    //
    // SetInputLayout(m_ctx.inputLayout.Get());
    //
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, m_ctx.screenTexture);
    // glBindSampler(0, m_ctx.screenSampler);
    // SetRasterizerState(m_ctx.state.Get());
    //
    // glDisable(GL_DEPTH_TEST);
    // Draw(6);
    // glEnable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_ctx.frame);
    for (auto const& view : m_ctx.viewports) {
        float const x1 = view.x;
        float const y1 = view.y;
        float const x2 = view.x + view.width;
        float const y2 = view.y + view.height;
        glBlitFramebuffer(x1, y1, x2, y2, x1, y1, x2, y2, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}

void Graphics::ClearRenderTargetView(IGLWRRenderTargetView* pRenderTargetView, float const color[4]) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, pRenderTargetView->m_clearFrame);
    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, m_ctx.frame);
}

void Graphics::ClearDepthStencilView(IGLWRDepthStencilView* pDepthStencilView, GLWRClearFlag flags, float depth,
                                     uint8_t stencil) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, pDepthStencilView->m_clearFrame);
    glClearDepth(depth);
    glClearStencil(stencil);
    glClear(flags);

    glBindFramebuffer(GL_FRAMEBUFFER, m_ctx.frame);
}

void Graphics::GenerateMips(IGLWRShaderResourceView* pShaderResourceView)
{
    glBindTexture(pShaderResourceView->m_type, pShaderResourceView->m_id);
    glGenerateMipmap(pShaderResourceView->m_type);
}

glm::mat4 Graphics::GetViewProjectionMatrix() const
{
    return m_camera.ViewProjectionMatrix();
}

glm::vec3 Graphics::GetCameraPosition() const
{
    return m_camera.position;
}

void Graphics::SetCamera(Camera camera)
{
    m_camera = camera;
}

Camera& Graphics::GetCamera()
{
    return m_camera;
}

void Graphics::Map(IGLWRResource* pResource, GLWRMapPermission permission, GLWRMappedSubresource* pMappedResource)
{
    GLuint const id = pResource->m_id;

    switch (pResource->m_type) {
    case GLWRResourceType_Buffer:
        glBindBuffer(GL_ARRAY_BUFFER, id);
        pMappedResource->data = glMapBuffer(GL_ARRAY_BUFFER, permission);
        break;
    case GLWRResourceType_IndexBuffer:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        pMappedResource->data = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, permission);
        break;
    case GLWRResourceType_UniformBuffer:
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        pMappedResource->data = glMapBuffer(GL_UNIFORM_BUFFER, permission);
        break;
    default:
        log_error("Graphics::Map currently does not support this resource type.");
        pMappedResource->data = nullptr;
        break;
    }
}

void Graphics::Unmap(IGLWRResource* pResource)
{
    GLuint const id = pResource->m_id;

    switch (pResource->m_type) {
    case GLWRResourceType_Buffer:
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        break;
    case GLWRResourceType_IndexBuffer:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        break;
    case GLWRResourceType_UniformBuffer:
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
        break;
    default:
        log_error("Graphics::Unmap currently does not support this resource type.");
        break;
    }
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
    log_error("Shader program linking error: %s\n", log.data());
}

std::string Graphics::SlurpShaderSource(std::string const& filename)
{
    std::fstream file;
    std::string source;
    std::size_t size;

    file.open(filename, std::fstream::in | std::fstream::binary);
    if (file.fail()) {
        log_error("Failed to open shader file: %s", filename.c_str());
    }

    file.seekg(0, std::fstream::end);
    size = file.tellg();
    file.seekg(0);

    source.resize(size);
    file.read(source.data(), size);

    file.close();

    return source;
}

void Graphics::CreateShaderResourceViewFromFile(Graphics* pContext, char const* filename,
                                                IGLWRShaderResourceView** ppResourceView)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(0);
    stbi_uc* image = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

    if (image == nullptr) {
        log_error("Failed to open image: %s. %s", filename, stbi_failure_reason());
    }

    GLWRPtr<IGLWRTexture2D> texture;

    GLWRTexture2DDesc texDesc;
    texDesc.width = width;
    texDesc.height = height;
    texDesc.internalFormat = GL_RGBA32F;
    texDesc.pixelFormat = GL_RGBA;
    texDesc.dataType = GL_UNSIGNED_BYTE;

    GLWRResourceData resData;
    resData.mem = image;

    GLWRShaderResourceViewDesc viewDesc;
    viewDesc.format = GL_RGBA32F;
    viewDesc.type = GLWRShaderResourceViewType_Texture2D;

    pContext->CreateTexture2D(&texDesc, &resData, &texture);
    pContext->CreateShaderResourceView(texture.Get(), &viewDesc, ppResourceView);

    stbi_image_free(image);
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
