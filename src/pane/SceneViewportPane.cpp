#include <cassert>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>

#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "World.hpp"
#include "common/Logger.hpp"
#include "gfx/Renderer.hpp"
#include "pane/SceneViewportPane.hpp"

#include "Vertex.hpp"

enum {
    Pane_SceneViewport = wxID_HIGHEST + 1,
};

wxBEGIN_EVENT_TABLE(SceneViewportPane, wxGLCanvas)
    EVT_PAINT(SceneViewportPane::OnPaint)
    EVT_SIZE(SceneViewportPane::OnSize)
    EVT_MOTION(SceneViewportPane::OnMouseMotion)
    EVT_LEFT_DOWN(SceneViewportPane::OnMouseLeftDown)
    EVT_RIGHT_DOWN(SceneViewportPane::OnMouseRightDown)
    EVT_MOUSEWHEEL(SceneViewportPane::OnMouseWheel)
    EVT_UPDATE_UI(Pane_SceneViewport, SceneViewportPane::OnUpdateUI)
wxEND_EVENT_TABLE()

float static constexpr MATH_PI = 3.14159265f;
float static constexpr MATH_2_MUL_PI = 2.0f * MATH_PI;

// Register factory: OpenGLCanvas
static WatermarkingProject::AttachedWindows::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> wxWeakRef<wxWindow> {
        auto& window = ProjectWindow::Get(project);
        wxWindow* mainPage = window.GetMainPage();
        wxASSERT(mainPage != nullptr);

        wxGLAttributes attrs;
        attrs.PlatformDefaults().MinRGBA(8, 8, 8, 8).DoubleBuffer().Depth(24).EndList();
        return new SceneViewportPane(mainPage, attrs, Pane_SceneViewport, wxDefaultPosition, wxDefaultSize, project);
    }
};

SceneViewportPane& SceneViewportPane::Get(WatermarkingProject& project)
{
    return project.AttachedWindows::Get<SceneViewportPane>(factoryKey);
}

SceneViewportPane const& SceneViewportPane::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

SceneViewportPane::SceneViewportPane(wxWindow* parent, wxGLAttributes const& dispAttrs, wxWindowID id,
                                     wxPoint const& pos, wxSize const& size, WatermarkingProject& project)
    : wxGLCanvas(parent, dispAttrs, id, pos, size)
    , m_project(project)
    , m_context(nullptr)
    , m_isGLLoaded(false)
    , m_rateMove(0.4f)
    , m_rateRotate(0.005f)
    , m_dirHorizontal(1)
{
    wxGLContextAttrs attrs;
    attrs.CoreProfile().OGLVersion(4, 3).Robust().EndList();
    m_context = std::make_unique<wxGLContext>(this, nullptr, &attrs);
}

SceneViewportPane::~SceneViewportPane()
{
}

void SceneViewportPane::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);
    SetCurrent(*m_context);

    auto& gfx = Graphics::Get(m_project);

    float bgColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
    gfx.ClearRenderTargetView(m_rtv.Get(), bgColor);
    gfx.ClearDepthStencilView(m_dsv.Get(), GLWRClearFlag_Depth);

    m_project.BuildLatticeMesh();
    Renderer::Get(m_project).Render(gfx);

    gfx.Present();

    SwapBuffers();
}

void SceneViewportPane::InitGL()
{
    SetCurrent(*m_context);
    m_isGLLoaded = gladLoadGL();
    assert(m_isGLLoaded);

    auto& gfx = Graphics::Get(m_project);
    InitFrame(gfx);

#ifndef NDEBUG
    glDebugMessageCallback(
        []([[maybe_unused]] GLenum source, [[maybe_unused]] GLenum type, [[maybe_unused]] GLuint id,
           [[maybe_unused]] GLenum severity, [[maybe_unused]] GLsizei length, [[maybe_unused]] GLchar const* message,
           [[maybe_unused]] void const* userParam) noexcept {
            std::cerr << std::hex << "[Type " << type << "]"
                      << "[Severity " << severity << "]"
                      << " Message: " << message << "\n";
        },
        nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

    GLWRBlendDesc blendDesc;
    blendDesc.enable = true;
    blendDesc.srcRGB = GLWRBlend_SrcAlpha;
    blendDesc.dstRGB = GLWRBlend_OneMinusSrcAlpha;
    blendDesc.eqRGB = GLWRBlendEq_Add;
    blendDesc.srcAlpha = GLWRBlend_One;
    blendDesc.dstAlpha = GLWRBlend_Zero;
    blendDesc.eqAlpha = GLWRBlendEq_Add;

    GLWRPtr<IGLWRBlendState> blend;
    gfx.CreateBlendState(&blendDesc, &blend);
    gfx.SetBlendState(blend.Get());

    std::cout << "Version:      " << glGetString(GL_VERSION) << "\n"
              << "Graphics:     " << glGetString(GL_RENDERER) << "\n"
              << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
              << "Vendor:       " << glGetString(GL_VENDOR) << std::endl;
}

void SceneViewportPane::OnSize(wxSizeEvent&)
{
    assert(m_context.get() != nullptr);

    // Guard for SetCurrent and calling GL functions
    if (!IsShownOnScreen() || !m_isGLLoaded)
        return;

    wxSize const size = GetClientSize() * GetContentScaleFactor();
    Graphics::Get(m_project).GetCamera().aspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);

    SetCurrent(*m_context);

    auto& gfx = Graphics::Get(m_project);

    InitFrame(gfx);

    GLWRViewport v;
    v.x = 0.0f;
    v.y = 0.0f;
    v.width = size.x;
    v.height = size.y;
    v.nearDepth = 0.0;
    v.farDepth = 1.0;

    gfx.SetViewports(1, &v);
}

void SceneViewportPane::OnMouseWheel(wxMouseEvent& event)
{
    int direction = event.GetWheelRotation() / 120;
    float& zoom = Graphics::Get(m_project).GetCamera().zoom;
    float const tmp_zoom = zoom + direction * -0.02f;
    constexpr float min = 0.01f;
    constexpr float max = 10.0f;

    if (tmp_zoom < min) {
        zoom = min;
    } else if (tmp_zoom >= max) {
        zoom = max;
    } else {
        zoom = tmp_zoom;
    }
}

void SceneViewportPane::OnMouseLeftDown(wxMouseEvent& event)
{
    wxCoord const x = event.GetX();
    wxCoord const y = event.GetY();
    m_originTranslate = std::make_tuple(x, y, Graphics::Get(m_project).GetCamera().center);
}

void SceneViewportPane::OnMouseRightDown(wxMouseEvent& event)
{
    wxCoord const x = event.GetX();
    wxCoord const y = event.GetY();
    auto& coord = Graphics::Get(m_project).GetCamera().coord;
    // Change rotating direction if the camera passes through the polars.
    // Theta is guaranteed to be on either [0, pi] or (pi, 2 * pi).
    if (coord.theta > MATH_PI)
        m_dirHorizontal = -1;
    else
        m_dirHorizontal = 1;

    m_originRotate = std::make_tuple(x, y, coord.phi, coord.theta);
}

void SceneViewportPane::OnMouseMotion(wxMouseEvent& event)
{
    wxCoord const x = event.GetX();
    wxCoord const y = event.GetY();

    auto& cam = Graphics::Get(m_project).GetCamera();

    if (event.LeftIsDown()) {
        auto const& [oX, oY, oTarget] = m_originTranslate;
        cam.center = oTarget + (-(x - oX) * cam.basis.sideway + (y - oY) * cam.basis.up) * m_rateMove * cam.zoom;
        cam.UpdateViewCoord();
    }
    if (event.RightIsDown()) {
        auto const& [oX, oY, oPhi, oTheta] = m_originRotate;
        auto& coord = cam.coord;
        coord.phi = RoundGuard(m_dirHorizontal * -(x - oX) * m_rateRotate + oPhi);
        coord.theta = RoundGuard(-(y - oY) * m_rateRotate + oTheta);
        cam.UpdateViewCoord();
    }
}

void SceneViewportPane::OnUpdateUI(wxUpdateUIEvent&)
{
    Refresh();
}

void SceneViewportPane::ResetCamera()
{
    wxSize const size = GetClientSize() * GetContentScaleFactor();
    Graphics::Get(m_project).GetCamera() = Camera(size.x, size.y);
}

// Restrict both phi and theta within 0 and 360 degrees.
float SceneViewportPane::RoundGuard(float radian)
{
    if (radian < 0.0f)
        return radian + MATH_2_MUL_PI;
    else if (radian > MATH_2_MUL_PI)
        return radian - MATH_2_MUL_PI;
    else
        return radian;
}

void SceneViewportPane::InitFrame(Graphics& gfx)
{
    m_rtv = GLWRPtr<IGLWRRenderTargetView>();
    m_dsv = GLWRPtr<IGLWRDepthStencilView>();

    wxSize const size = GetClientSize() * GetContentScaleFactor();

    {
        IGLWRTexture2D* texture = nullptr;
        GLWRTexture2DDesc texDesc;
        GLWRResourceData initData;
        texDesc.internalFormat = GL_RGB32F;
        texDesc.pixelFormat = GL_RGB;
        texDesc.dataType = GL_UNSIGNED_BYTE;
        texDesc.width = size.x;
        texDesc.height = size.y;
        initData.mem = nullptr;
        gfx.CreateTexture2D(&texDesc, &initData, &texture);

        GLWRRenderTargetViewDesc viewDesc;
        viewDesc.type = GLWRRenderTargetViewType_Texture2D;
        viewDesc.internalFormat = GL_RGB32F;
        gfx.CreateRenderTargetView(texture, &viewDesc, &m_rtv);
        texture->Release();
    }
    {
        IGLWRRenderBuffer* buffer = nullptr;
        GLWRRenderBufferDesc desc;
        desc.internalFormat = GL_DEPTH_STENCIL;
        desc.width = size.x;
        desc.height = size.y;
        gfx.CreateRenderBuffer(&desc, &buffer);

        GLWRDepthStencilViewDesc viewDesc;
        viewDesc.type = GLWRDepthStencilViewType_RenderBuffer;
        viewDesc.internalFormat = GL_DEPTH24_STENCIL8;
        gfx.CreateDepthStencilView(buffer, &viewDesc, &m_dsv);
        buffer->Release();
    }

    gfx.SetRenderTargets(1, &m_rtv, m_dsv.Get());
}
