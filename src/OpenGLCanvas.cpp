#include "OpenGLCanvas.hpp"
#include "assetlib/OBJ/OBJImporter.hpp"
#include "assetlib/STL/STLImporter.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>

#include "World.hpp"

constexpr std::size_t static SIZE_UINT = sizeof(unsigned int);

OpenGLCanvas::OpenGLCanvas(wxWindow* parent, wxGLAttributes const& dispAttrs, wxWindowID id, wxPoint const& pos,
                           wxSize const& size, long style, wxString const& name)
    : wxGLCanvas(parent, dispAttrs, id, pos, size, style, name)
    , isGLLoaded_(false)
    , isAcceptingInput_(false)
    , iterPerFrame_(10)
    , RNG_(nullptr)
    , context_(nullptr)
    , lattice_(nullptr)
    , renderer_(nullptr)
{
    wxGLContextAttrs attrs;
    attrs.CoreProfile().OGLVersion(4, 3).Robust().EndList();
    context_ = std::make_unique<wxGLContext>(this, nullptr, &attrs);
    lattice_ = std::make_unique<Lattice>(64, 64, 50000, 0.15f);
}

OpenGLCanvas::~OpenGLCanvas() { }

void OpenGLCanvas::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);
    SetCurrent(*context_);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UpdateLatticePositions();

    if (rendopt & RenderOption_LatticeFace) {
        UpdateLatticeFaces();
    }

    for (int i = 0; i < iterPerFrame_; ++i) {
        if (isAcceptingInput_) {
            auto const& p = world.surface.Vertices()[RNG_->scalar()].position;
            isAcceptingInput_ = lattice_->Input(std::vector { p.x, p.y, p.z });
        }
    }

    renderer_->Render();

    SwapBuffers();
}

void OpenGLCanvas::InitGL()
{
    SetCurrent(*context_);

    isGLLoaded_ = gladLoadGL();

    assert(isGLLoaded_);

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

    wxSize const size = GetClientSize() * GetContentScaleFactor();
    assert(size.x > 0 && size.y > 0);

    OBJImporter obj;
    obj.Read("res/models/surface2.obj");
    world.surface.Import(obj.Model());

    STLImporter stl;
    stl.Read("res/models/UVSphere.stl");
    world.uvsphere.Import(stl.Model());

    world.lattice.indices = lattice_->EdgeIndices();
    UpdateLatticePositions();
    UpdateLatticeFaces();

    renderer_ = std::make_unique<Renderer>(size.x, size.y);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::cout << "Version:      " << glGetString(GL_VERSION) << "\n"
              << "Renderer:     " << glGetString(GL_RENDERER) << "\n"
              << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
              << "Vendor:       " << glGetString(GL_VENDOR) << std::endl;

    RNG_ = std::make_unique<RandomIntNumber<unsigned int>>(0, world.surface.Vertices().size() - 1);
}

void OpenGLCanvas::OnSize(wxSizeEvent&)
{
    assert(context_.get() != nullptr);

    wxSize const size = GetClientSize() * GetContentScaleFactor();

    if (renderer_ != nullptr) {
        renderer_->GetCamera().SetAspectRatio(size.x, size.y);
    }

    // Guard for SetCurrent and calling GL functions
    if (!IsShownOnScreen() || !isGLLoaded_)
        return;

    SetCurrent(*context_);
    glViewport(0, 0, size.x, size.y);
}

void OpenGLCanvas::OnMouseWheel(wxMouseEvent& event)
{
    renderer_->GetCamera().WheelZoom(event.GetWheelRotation() / 120);
}

void OpenGLCanvas::OnMouseLeftDown(wxMouseEvent& event)
{
    wxCoord const x = event.GetX();
    wxCoord const y = event.GetY();
    renderer_->GetCamera().InitDragTranslation(x, y);
}

void OpenGLCanvas::OnMouseRightDown(wxMouseEvent& event)
{
    wxCoord const x = event.GetX();
    wxCoord const y = event.GetY();
    renderer_->GetCamera().InitDragRotation(x, y);
}

void OpenGLCanvas::OnMouseMotion(wxMouseEvent& event)
{
    wxCoord const x = event.GetX();
    wxCoord const y = event.GetY();
    if (event.LeftIsDown()) {
        renderer_->GetCamera().DragTranslation(x, y);
    }
    if (event.RightIsDown()) {
        renderer_->GetCamera().DragRotation(x, y);
    }
}

void OpenGLCanvas::ResetCamera()
{
    wxSize const size = GetClientSize() * GetContentScaleFactor();
    renderer_->GetCamera() = Camera(size.x, size.y);
}

void OpenGLCanvas::OpenSurface(std::string const& path)
{
    OBJImporter obj;
    obj.Read(path);
    world.surface.Import(obj.Model());
    RNG_ = std::make_unique<RandomIntNumber<unsigned int>>(0, world.surface.Vertices().size() - 1);
    renderer_->LoadLattice();
}

void OpenGLCanvas::UpdateLatticePositions()
{
    world.lattice.positions.clear();
    auto const& neurons = lattice_->Neurons();
    world.lattice.positions.reserve(neurons.size());
    for (Node const& n : neurons) {
        world.lattice.positions.emplace_back(n[0], n[1], n[2]);
    }
}

void OpenGLCanvas::UpdateLatticeFaces()
{
    world.lattice.faces.clear();
    auto const& indices = lattice_->FaceIndices();
    world.lattice.faces.reserve(indices.size());
    for (std::size_t i = 0; i < indices.size(); i += 3) {
        auto const p1 = world.lattice.positions[indices[i]];
        auto const p2 = world.lattice.positions[indices[i + 1]];
        auto const p3 = world.lattice.positions[indices[i + 2]];

        glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
        if (std::isnan(normal.x) || std::isnan(normal.y) || std::isnan(normal.z)) {
            normal = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        world.lattice.faces.push_back(Vertex { p1, normal });
        world.lattice.faces.push_back(Vertex { p2, normal });
        world.lattice.faces.push_back(Vertex { p3, normal });
    }
}

void OpenGLCanvas::SetPlayOrPause(bool isAcceptingInput)
{
    isAcceptingInput_ = isAcceptingInput;
}

void OpenGLCanvas::ToggleRenderOption(RenderOption opt)
{
    if (rendopt & opt) {
        rendopt -= opt;
    } else {
        rendopt += opt;
    }
}

bool OpenGLCanvas::GetRenderOptionState(RenderOption opt) const
{
    return rendopt & opt;
}

int OpenGLCanvas::GetCurrentIterations() const
{
    return lattice_->CurrentIteration();
}

void OpenGLCanvas::ResetLattice(int width, int height, int iterationCap, float initLearningRate)
{
    isAcceptingInput_ = false;
    bool const isSameWidth = (width == lattice_->Width());
    bool const isSameHeight = (height == lattice_->Height());
    bool const isSameIter = (iterationCap == lattice_->IterationCap());
    bool const isSameRate = (initLearningRate == lattice_->InitialRate());
    bool const changed = !(isSameWidth && isSameHeight && isSameIter && isSameRate);
    lattice_ = std::make_unique<Lattice>(width, height, iterationCap, initLearningRate);

    if (changed) {
        world.lattice.indices = lattice_->EdgeIndices();
        UpdateLatticePositions();
        UpdateLatticeFaces();
        renderer_->LoadLattice();
    }
}

int OpenGLCanvas::GetIterationCap() const
{
    assert(lattice_.get() != nullptr);
    return lattice_->IterationCap();
}

float OpenGLCanvas::GetInitialLearningRate() const
{
    assert(lattice_.get() != nullptr);
    return lattice_->InitialRate();
}

int OpenGLCanvas::GetLatticeWidth() const
{
    assert(lattice_.get() != nullptr);
    return lattice_->Width();
}

int OpenGLCanvas::GetLatticeHeight() const
{
    assert(lattice_.get() != nullptr);
    return lattice_->Height();
}

void OpenGLCanvas::SetSurfaceColorAlpha(float alpha)
{
    // The range between 0.0 and 1.0 should be handled by UI
    world.surfaceColorAlpha = alpha;
}

float OpenGLCanvas::GetSurfaceTransparency() const
{
    return world.surfaceColorAlpha;
}

void OpenGLCanvas::SetIterationsPerFrame(int times)
{
    iterPerFrame_ = times;
}

int OpenGLCanvas::GetIterationsPerFrame() const
{
    return iterPerFrame_;
}

wxBEGIN_EVENT_TABLE(OpenGLCanvas, wxGLCanvas)
    EVT_PAINT(OpenGLCanvas::OnPaint)
    EVT_SIZE(OpenGLCanvas::OnSize)
    EVT_MOTION(OpenGLCanvas::OnMouseMotion)
    EVT_LEFT_DOWN(OpenGLCanvas::OnMouseLeftDown)
    EVT_RIGHT_DOWN(OpenGLCanvas::OnMouseRightDown)
    EVT_MOUSEWHEEL(OpenGLCanvas::OnMouseWheel)
wxEND_EVENT_TABLE()
