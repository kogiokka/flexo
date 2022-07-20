#include <cassert>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <limits>

#include <glm/gtx/string_cast.hpp>

#include "InputData.hpp"
#include "Lattice.hpp"
#include "Mesh.hpp"
#include "OpenGLCanvas.hpp"
#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "World.hpp"
#include "common/Logger.hpp"

wxDEFINE_EVENT(CMD_REBUILD_LATTICE_MESH, wxCommandEvent);

enum {
    TIMER_CANVAS_UPDATE,
};

wxBEGIN_EVENT_TABLE(OpenGLCanvas, wxGLCanvas)
    EVT_PAINT(OpenGLCanvas::OnPaint)
    EVT_SIZE(OpenGLCanvas::OnSize)
    EVT_MOTION(OpenGLCanvas::OnMouseMotion)
    EVT_LEFT_DOWN(OpenGLCanvas::OnMouseLeftDown)
    EVT_RIGHT_DOWN(OpenGLCanvas::OnMouseRightDown)
    EVT_MOUSEWHEEL(OpenGLCanvas::OnMouseWheel)
    EVT_TIMER(TIMER_CANVAS_UPDATE, OpenGLCanvas::OnUpdateTimer)
wxEND_EVENT_TABLE()

float static constexpr MATH_PI = 3.14159265f;
float static constexpr MATH_2_MUL_PI = 2.0f * MATH_PI;

static WatermarkingProject::AttachedWindows::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> wxWeakRef<wxWindow> {
        auto& window = ProjectWindow::Get(project);
        wxWindow* mainPage = window.GetMainPage();
        wxASSERT(mainPage != nullptr);

        wxGLAttributes attrs;
        attrs.PlatformDefaults().MinRGBA(8, 8, 8, 8).DoubleBuffer().Depth(24).EndList();
        auto canvas = new OpenGLCanvas(mainPage, attrs, wxID_ANY, wxDefaultPosition, wxDefaultSize);
        project.SetPanel(canvas);

        return canvas;
    }
};

OpenGLCanvas& OpenGLCanvas::Get(WatermarkingProject& project)
{
    return project.AttachedWindows::Get<OpenGLCanvas>(factoryKey);
}

OpenGLCanvas const& OpenGLCanvas::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

OpenGLCanvas::OpenGLCanvas(wxWindow* parent, wxGLAttributes const& dispAttrs, wxWindowID id, wxPoint const& pos,
                           wxSize const& size, long style, wxString const& name)
    : wxGLCanvas(parent, dispAttrs, id, pos, size, style, name)
    , m_isGLLoaded(false)
    , m_context(nullptr)
    , m_renderer(nullptr)
    , m_rateMove(0.4f)
    , m_rateRotate(0.005f)
    , m_dirHorizontal(1)
{
    wxGLContextAttrs attrs;
    attrs.CoreProfile().OGLVersion(4, 3).Robust().EndList();
    m_context = std::make_unique<wxGLContext>(this, nullptr, &attrs);

    m_updateTimer = new wxTimer(this, TIMER_CANVAS_UPDATE);
    m_updateTimer->Start(16); // 16 ms (60 fps)
}

OpenGLCanvas::~OpenGLCanvas()
{
    m_updateTimer->Stop();
}

void OpenGLCanvas::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);
    SetCurrent(*m_context);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UpdateScene();

    m_renderer->Render();

    SwapBuffers();
}

void OpenGLCanvas::InitGL()
{
    SetCurrent(*m_context);
    m_isGLLoaded = gladLoadGL();
    assert(m_isGLLoaded);

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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::cout << "Version:      " << glGetString(GL_VERSION) << "\n"
              << "Renderer:     " << glGetString(GL_RENDERER) << "\n"
              << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
              << "Vendor:       " << glGetString(GL_VENDOR) << std::endl;
}

void OpenGLCanvas::SetRenderer(std::shared_ptr<Renderer> renderer)
{
    m_renderer = renderer;
}

void OpenGLCanvas::OnSize(wxSizeEvent&)
{
    assert(m_context.get() != nullptr);

    wxSize const size = GetClientSize() * GetContentScaleFactor();

    if (m_renderer) {
        m_renderer->GetCamera().aspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);
    }

    // Guard for SetCurrent and calling GL functions
    if (!IsShownOnScreen() || !m_isGLLoaded)
        return;

    SetCurrent(*m_context);
    glViewport(0, 0, size.x, size.y);
}

void OpenGLCanvas::OnMouseWheel(wxMouseEvent& event)
{
    int direction = event.GetWheelRotation() / 120;
    float& zoom = m_renderer->GetCamera().zoom;
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

void OpenGLCanvas::OnMouseLeftDown(wxMouseEvent& event)
{
    wxCoord const x = event.GetX();
    wxCoord const y = event.GetY();
    m_originTranslate = std::make_tuple(x, y, m_renderer->GetCamera().center);
}

void OpenGLCanvas::OnMouseRightDown(wxMouseEvent& event)
{
    wxCoord const x = event.GetX();
    wxCoord const y = event.GetY();
    auto& coord = m_renderer->GetCamera().coord;
    // Change rotating direction if the camera passes through the polars.
    // Theta is guaranteed to be on either [0, pi] or (pi, 2 * pi).
    if (coord.theta > MATH_PI)
        m_dirHorizontal = -1;
    else
        m_dirHorizontal = 1;

    m_originRotate = std::make_tuple(x, y, coord.phi, coord.theta);
}

void OpenGLCanvas::OnMouseMotion(wxMouseEvent& event)
{
    wxCoord const x = event.GetX();
    wxCoord const y = event.GetY();

    auto& cam = m_renderer->GetCamera();

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

void OpenGLCanvas::OnUpdateTimer(wxTimerEvent&)
{
    Refresh();
}

void OpenGLCanvas::ResetCamera()
{
    wxSize const size = GetClientSize() * GetContentScaleFactor();
    m_renderer->GetCamera() = Camera(size.x, size.y);
}

void OpenGLCanvas::UpdateScene()
{
    if (world.renderFlags & RenderFlag_DrawLatticeVertex || world.renderFlags & RenderFlag_DrawLatticeEdge
        || world.renderFlags & RenderFlag_DrawLatticeFace) {
        wxCommandEvent event(CMD_REBUILD_LATTICE_MESH, GetId());
        ProcessWindowEvent(event);
    }
}

// Restrict both phi and theta within 0 and 360 degrees.
float OpenGLCanvas::RoundGuard(float radian)
{
    if (radian < 0.0f)
        return radian + MATH_2_MUL_PI;
    else if (radian > MATH_2_MUL_PI)
        return radian - MATH_2_MUL_PI;
    else
        return radian;
}
