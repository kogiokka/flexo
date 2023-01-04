#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iostream>

#include <stb_image_write.h>

#include "Camera.hpp"
#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "gfx/Renderer.hpp"
#include "object/ObjectList.hpp"
#include "pane/SceneViewportPane.hpp"
#include "util/Logger.h"

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
static FlexoProject::AttachedWindows::RegisteredFactory const factoryKey {
    [](FlexoProject& project) -> wxWeakRef<wxWindow> {
        auto& window = ProjectWindow::Get(project);
        wxWindow* mainPage = window.GetMainPage();
        wxASSERT(mainPage != nullptr);

        wxGLAttributes attrs;
        attrs.PlatformDefaults().MinRGBA(8, 8, 8, 8).DoubleBuffer().Depth(24).EndList();
        return new SceneViewportPane(mainPage, attrs, Pane_SceneViewport, wxDefaultPosition, wxDefaultSize, project);
    }
};

SceneViewportPane& SceneViewportPane::Get(FlexoProject& project)
{
    return project.AttachedWindows::Get<SceneViewportPane>(factoryKey);
}

SceneViewportPane const& SceneViewportPane::Get(FlexoProject const& project)
{
    return Get(const_cast<FlexoProject&>(project));
}

SceneViewportPane::SceneViewportPane(wxWindow* parent, wxGLAttributes const& dispAttrs, wxWindowID id,
                                     wxPoint const& pos, wxSize const& size, FlexoProject& project)
    : wxGLCanvas(parent, dispAttrs, id, pos, size)
    , m_isGLLoaded(false)
    , m_dirHorizontal(1)
    , m_context(nullptr)
    , m_project(project)
{
    wxGLContextAttrs attrs;
    attrs.CoreProfile().OGLVersion(4, 3).Robust().EndList();
    m_context = std::make_unique<wxGLContext>(this, nullptr, &attrs);

    m_project.Bind(EVT_SCREENSHOT, &SceneViewportPane::OnMenuScreenshot, this);

    m_project.Bind(EVT_MENU_CAMERA_PERSPECTIVE, [this](wxCommandEvent&) {
        auto& cam = Graphics::Get(m_project).GetCamera();
        cam.SetProjectionMode(Camera::ProjectionMode::Perspective);
    });
    m_project.Bind(EVT_MENU_CAMERA_ORTHOGONAL, [this](wxCommandEvent&) {
        auto& cam = Graphics::Get(m_project).GetCamera();
        cam.SetProjectionMode(Camera::ProjectionMode::Orthogonal);
    });
}

SceneViewportPane::~SceneViewportPane()
{
}

void SceneViewportPane::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);
    SetCurrent(*m_context);

    auto& gfx = Graphics::Get(m_project);

    float bgColor[4] = { 0.2157f, 0.2157f, 0.2157f, 1.0f };
    gfx.ClearRenderTargetView(m_rtv.Get(), bgColor);
    gfx.ClearDepthStencilView(m_dsv.Get(), GLWRClearFlag_Depth);

    auto& renderer = Renderer::Get(m_project);
    if (auto it = m_project.theMap.lock()) {
        it->GenerateMesh();
        it->GenerateDrawables(gfx);
        ObjectList::Get(m_project).Submit(renderer);
    }

    renderer.Render(gfx);

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

    gfx.SetCamera(CreateDefaultCamera());
}

void SceneViewportPane::ResetCamera()
{
    Graphics::Get(m_project).SetCamera(CreateDefaultCamera());
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
    ProjectWindow::Get(m_project).SetStatusText(wxString::Format("Viewport size: %dx%d", size.x, size.y));
}

void SceneViewportPane::OnMouseWheel(wxMouseEvent& event)
{
    auto& cam = Graphics::Get(m_project).GetCamera();
    int const diff = -1 * event.GetWheelRotation() / 120;
    cam.Zoom(diff);
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
    auto coord = Graphics::Get(m_project).GetCamera().coord.lock();
    // Change rotating direction if the camera passes through the polars.
    // Theta is guaranteed to be on either [0, pi] or (pi, 2 * pi).
    if (coord->theta > MATH_PI)
        m_dirHorizontal = -1;
    else
        m_dirHorizontal = 1;

    m_originRotate = std::make_tuple(x, y, coord->phi, coord->theta);
}

void SceneViewportPane::OnMouseMotion(wxMouseEvent& event)
{
    wxCoord const x = event.GetX();
    wxCoord const y = event.GetY();

    auto& cam = Graphics::Get(m_project).GetCamera();

    if (event.LeftIsDown()) {
        auto const& [oX, oY, oTarget] = m_originTranslate;
        float radius = cam.perspCoord->r;
        float dx = 0.001f * radius * -(x - oX);
        float dy = 0.001f * radius * (y - oY);
        cam.center = oTarget + (dx * cam.basis.sideway + dy * cam.basis.up);
        cam.UpdateViewCoord();
    }
    if (event.RightIsDown()) {
        auto const& [oX, oY, oPhi, oTheta] = m_originRotate;
        float dx = 0.003f * -(x - oX);
        float dy = 0.003f * (y - oY);
        float phi = RoundGuard(m_dirHorizontal * dx + oPhi);
        float theta = RoundGuard(-dy + oTheta);
        cam.orthoCoord->phi = phi;
        cam.orthoCoord->theta = theta;
        cam.perspCoord->phi = phi;
        cam.perspCoord->theta = theta;
        cam.UpdateViewCoord();
    }
}

void SceneViewportPane::OnMenuScreenshot(wxCommandEvent&)
{
    wxSize const size = GetClientSize() * GetContentScaleFactor();
    std::vector<uint8_t> image(size.x * size.y * 4);

    glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, image.data());

    std::time_t now = time(0);
    char filename[80];
    struct tm timeStruct = *localtime(&now);
    std::strftime(filename, sizeof(filename), "Screenshot_%Y%m%d_%H%M%S.png", &timeStruct);
    stbi_flip_vertically_on_write(1);
    stbi_write_png(filename, size.x, size.y, 4, image.data(), size.x * 4);
    ProjectWindow::Get(m_project).SetStatusText(wxString::Format("The screenshot was saved as \"%s\"", filename));
}

void SceneViewportPane::OnUpdateUI(wxUpdateUIEvent&)
{
    Refresh();
}

Camera SceneViewportPane::CreateDefaultCamera() const
{
    Camera camera;
    wxSize const size = GetClientSize() * GetContentScaleFactor();
    camera.aspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);

    return camera;
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
        texDesc.samples = 4;
        initData.mem = nullptr;
        gfx.CreateTexture2D(&texDesc, &initData, &texture);

        GLWRRenderTargetViewDesc viewDesc;
        viewDesc.type = GLWRRenderTargetViewType_Texture2D_MS;
        viewDesc.internalFormat = GL_RGB32F;
        gfx.CreateRenderTargetView(texture, &viewDesc, &m_rtv);
        texture->Release();
    }
    {
        IGLWRRenderBuffer* buffer = nullptr;
        GLWRRenderBufferDesc desc;
        desc.samples = 4;
        desc.internalFormat = GL_DEPTH_STENCIL;
        desc.width = size.x;
        desc.height = size.y;
        gfx.CreateRenderBuffer(&desc, &buffer);

        GLWRDepthStencilViewDesc viewDesc;
        viewDesc.type = GLWRDepthStencilViewType_RenderBuffer_MS;
        viewDesc.internalFormat = GL_DEPTH24_STENCIL8;
        gfx.CreateDepthStencilView(buffer, &viewDesc, &m_dsv);
        buffer->Release();
    }

    gfx.SetRenderTargets(1, m_rtv.GetAddressOf(), m_dsv.Get());
}
