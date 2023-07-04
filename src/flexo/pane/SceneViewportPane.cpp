#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <fstream>

#include <stb/image_write.h>

#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "Scene.hpp"
#include "SceneController.hpp"
#include "dialog/ViewportSettingsDialog.hpp"
#include "gfx/Camera.hpp"
#include "gfx/Renderer.hpp"
#include "log/Logger.h"
#include "pane/SceneViewportPane.hpp"

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

static void GLDebugProc(GLenum, GLenum type, GLuint, GLenum severity, GLsizei, GLchar const* message, void const*)
{
    log_debug("[Type: %X, Severity: %X] %s", type, severity, message);
}

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

    m_settings.background = BACKGROUND_DARK;
    m_settings.overlayFlags = Overlays_GuidesAxisX | Overlays_GuidesAxisY | Overlays_GuidesGrid;

    m_project.Bind(EVT_SCREENSHOT, &SceneViewportPane::OnMenuScreenshot, this);

    m_project.Bind(EVT_MENU_CAMERA_PERSPECTIVE, [this](wxCommandEvent&) {
        auto& cam = m_gfx->GetCamera();
        cam.SetProjectionMode(Camera::ProjectionMode::Perspective);
    });
    m_project.Bind(EVT_MENU_CAMERA_ORTHOGONAL, [this](wxCommandEvent&) {
        auto& cam = m_gfx->GetCamera();
        cam.SetProjectionMode(Camera::ProjectionMode::Orthogonal);
    });

    m_project.Bind(EVT_VIEWPORT_SETTINGS_BACKGROUND_DARK,
                   [this](wxCommandEvent&) { m_settings.background = BACKGROUND_DARK; });

    m_project.Bind(EVT_VIEWPORT_SETTINGS_BACKGROUND_LIGHT,
                   [this](wxCommandEvent&) { m_settings.background = BACKGROUND_LIGHT; });

    m_project.Bind(EVT_VIEWPORT_SETTINGS_OVERLAY_GUIDES, [this](wxCommandEvent& event) {
        OverlayFlags flags = 0;
        if (event.GetInt()) {
            flags = Overlays_GuidesAxisX | Overlays_GuidesAxisY | Overlays_GuidesGrid;
        }
        m_settings.overlayFlags = flags;
        m_overlays->SetFlags(flags);
    });
}

SceneViewportPane::~SceneViewportPane()
{
}

void SceneViewportPane::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);
    SetCurrent(*m_context);

    auto& gfx = *m_gfx;
    auto bg = glm::vec4(m_settings.background, 1.0f);
    gfx.ClearRenderTargetView(m_rtv.Get(), &bg.x);
    gfx.ClearDepthStencilView(m_dsv.Get(), GLWRClearFlag_Depth);

    if (auto map = m_currMap.lock()) {
        map->GenerateDrawables(gfx);
    }

    auto& renderer = *m_renderer;
    m_overlays->Submit(renderer);
    m_scene->SubmitDrawables(renderer);

    renderer.Render(gfx);
    gfx.Present();
    renderer.Clear();

    SwapBuffers();
}

void SceneViewportPane::InitGL()
{
    SetCurrent(*m_context);
    m_isGLLoaded = gladLoadGL();
    assert(m_isGLLoaded);

    m_renderer = std::make_shared<Renderer>();
    m_gfx = std::make_shared<Graphics>();
    InitFrame(*m_gfx);

#ifndef NDEBUG
    glDebugMessageCallback(GLDebugProc, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

    log_info("Version: %s", glGetString(GL_VERSION));
    log_info("Graphics: %s", glGetString(GL_RENDERER));
    log_info("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    log_info("Vendor: %s", glGetString(GL_VENDOR));

    GLWRBlendDesc blendDesc;
    blendDesc.enable = true;
    blendDesc.srcRGB = GLWRBlend_SrcAlpha;
    blendDesc.dstRGB = GLWRBlend_OneMinusSrcAlpha;
    blendDesc.eqRGB = GLWRBlendEq_Add;
    blendDesc.srcAlpha = GLWRBlend_One;
    blendDesc.dstAlpha = GLWRBlend_Zero;
    blendDesc.eqAlpha = GLWRBlendEq_Add;

    GLWRPtr<IGLWRBlendState> blend;
    m_gfx->CreateBlendState(&blendDesc, &blend);
    m_gfx->SetBlendState(blend.Get());
    m_gfx->SetCamera(CreateDefaultCamera());

    m_overlays = std::make_unique<Overlays>(*m_gfx);
}

void SceneViewportPane::SetCurrentMap(std::weak_ptr<Map<3, 2>> map)
{
    m_currMap = map;
}

void SceneViewportPane::SetScene(Scene const& scene)
{
    m_scene = &scene;
}

Graphics& SceneViewportPane::GetGL()
{
    return *m_gfx;
}

SceneViewportPane::Settings SceneViewportPane::GetSettings() const
{
    return m_settings;
}

void SceneViewportPane::ResetCamera()
{
    m_gfx->SetCamera(CreateDefaultCamera());
}

void SceneViewportPane::OnSize(wxSizeEvent&)
{
    assert(m_context.get() != nullptr);

    // Guard for SetCurrent and calling GL functions
    if (!IsShownOnScreen() || !m_isGLLoaded)
        return;

    wxSize const size = GetClientSize() * GetContentScaleFactor();

    m_gfx->GetCamera().aspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);

    SetCurrent(*m_context);
    InitFrame(*m_gfx);

    GLWRViewport v;
    v.x = 0.0f;
    v.y = 0.0f;
    v.width = size.x;
    v.height = size.y;
    v.nearDepth = 0.0;
    v.farDepth = 1.0;

    m_gfx->SetViewports(1, &v);
    m_project.GetWindow()->SetStatusText(wxString::Format("Viewport size: %dx%d", size.x, size.y));
}

void SceneViewportPane::OnMouseWheel(wxMouseEvent& event)
{
    auto& cam = m_gfx->GetCamera();
    int const diff = -1 * event.GetWheelRotation() / 120;
    cam.Zoom(diff);
}

void SceneViewportPane::OnMouseLeftDown(wxMouseEvent& event)
{
    wxCoord const x = event.GetX();
    wxCoord const y = event.GetY();
    m_originTranslate = std::make_tuple(x, y, m_gfx->GetCamera().center);
}

void SceneViewportPane::OnMouseRightDown(wxMouseEvent& event)
{
    wxCoord const x = event.GetX();
    wxCoord const y = event.GetY();
    auto coord = m_gfx->GetCamera().coord.lock();
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

    auto& cam = m_gfx->GetCamera();

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
    m_project.GetWindow()->SetStatusText(wxString::Format("The screenshot was saved as \"%s\"", filename));
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
