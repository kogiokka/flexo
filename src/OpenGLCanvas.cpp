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
#include "World.hpp"
#include "assetlib/OBJ/OBJImporter.hpp"
#include "assetlib/STL/STLImporter.hpp"
#include "common/Logger.hpp"

float static constexpr MATH_PI = 3.14159265f;
float static constexpr MATH_PI_DIV_4 = MATH_PI * 0.25f;
float static constexpr MATH_2_MUL_PI = 2.0f * MATH_PI;

OpenGLCanvas::OpenGLCanvas(wxWindow* parent, wxGLAttributes const& dispAttrs, wxWindowID id, wxPoint const& pos,
                           wxSize const& size, long style, wxString const& name)
    : wxGLCanvas(parent, dispAttrs, id, pos, size, style, name)
    , isGLLoaded_(false)
    , context_(nullptr)
    , renderer_(nullptr)
    , rateMove_(0.4f)
    , rateRotate_(0.005f)
    , dirHorizontal_(1)
{
    wxGLContextAttrs attrs;
    attrs.CoreProfile().OGLVersion(4, 3).Robust().EndList();
    context_ = std::make_unique<wxGLContext>(this, nullptr, &attrs);
}

OpenGLCanvas::~OpenGLCanvas() { }

void OpenGLCanvas::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);
    SetCurrent(*context_);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UpdateScene();

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

    STLImporter stlImp;
    world.uvsphere = stlImp.ReadFile("res/models/UVSphere.stl");
    world.cube = stlImp.ReadFile("res/models/cube.stl");

    BuildLatticeMesh();
    UpdateLatticeEdges();

    renderer_ = std::make_unique<Renderer>(size.x, size.y);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::cout << "Version:      " << glGetString(GL_VERSION) << "\n"
              << "Renderer:     " << glGetString(GL_RENDERER) << "\n"
              << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
              << "Vendor:       " << glGetString(GL_VENDOR) << std::endl;
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
    assert(direction == 1 || direction == -1);

    int direction = event.GetWheelRotation() / 120;
    float& zoom = renderer_->GetCamera().GetZoom();
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
    originTranslate_ = std::make_tuple(x, y, renderer_->GetCamera().GetCenter());
}

void OpenGLCanvas::OnMouseRightDown(wxMouseEvent& event)
{
    wxCoord const x = event.GetX();
    wxCoord const y = event.GetY();
    auto& coord = renderer_->GetCamera().GetCoordinates();
    // Change rotating direction if the camera passes through the polars.
    // Theta is guaranteed to be on either [0, pi] or (pi, 2 * pi).
    if (coord.theta > MATH_PI)
        dirHorizontal_ = -1;
    else
        dirHorizontal_ = 1;

    originRotate_ = std::make_tuple(x, y, coord.phi, coord.theta);
}

void OpenGLCanvas::OnMouseMotion(wxMouseEvent& event)
{
    wxCoord const x = event.GetX();
    wxCoord const y = event.GetY();

    auto& cam = renderer_->GetCamera();

    if (event.LeftIsDown()) {
        auto const& [oX, oY, oTarget] = originTranslate_;
        cam.GetCenter()
            = oTarget + (-(x - oX) * cam.GetVectorSide() + (y - oY) * cam.GetVectorUp()) * rateMove_ * cam.GetZoom();
        cam.UpdateViewCoord();
    }
    if (event.RightIsDown()) {
        auto const& [oX, oY, oPhi, oTheta] = originRotate_;
        auto& coord = cam.GetCoordinates();
        coord.phi = RoundGuard(dirHorizontal_ * -(x - oX) * rateRotate_ + oPhi);
        coord.theta = RoundGuard(-(y - oY) * rateRotate_ + oTheta);
        cam.UpdateViewCoord();
    }
}

void OpenGLCanvas::ResetCamera()
{
    wxSize const size = GetClientSize() * GetContentScaleFactor();
    renderer_->GetCamera() = Camera(size.x, size.y);
}

void OpenGLCanvas::OpenInputDataFile(wxString const& path)
{
    world.polyModel = nullptr;
    world.volModel = nullptr;

    std::vector<glm::vec3> posData;

    if (path.EndsWith(".obj")) {
        OBJImporter objImp;
        world.polyModel = std::make_unique<Mesh>(objImp.ReadFile(path.ToStdString()));
        renderer_->LoadPolygonalModel();
    } else if (path.EndsWith(".stl")) {
        STLImporter stlImp;
        world.polyModel = std::make_unique<Mesh>(stlImp.ReadFile(path.ToStdString()));
        renderer_->LoadPolygonalModel();
    } else if (path.EndsWith(".toml")) {
        world.volModel = std::make_unique<VolModel>();
        auto& [data, pos, texcoord] = *world.volModel;

        if (!data.read(path.ToStdString())) {
            Logger::error(R"(Failed to read volumetric model: "%s")", path.ToStdString().c_str());
            return;
        }

        const int model = 252;
        const int isovalue = 0;
        auto reso = world.volModel->data.resolution();
        for (int x = 0; x < reso.x; x++) {
            for (int y = 0; y < reso.y; y++) {
                for (int z = 0; z < reso.z; z++) {
                    if (data.value(x, y, z) >= model) {
                        if (x + 1 < reso.x) {
                            if (data.value(x + 1, y, z) <= isovalue) {
                                pos.push_back(glm::vec3 { x, y, z });
                                continue;
                            }
                        }
                        if (x - 1 >= 0) {
                            if (data.value(x - 1, y, z) <= isovalue) {
                                pos.push_back(glm::vec3 { x, y, z });
                                continue;
                            }
                        }
                        if (y + 1 < reso.y) {
                            if (data.value(x, y + 1, z) <= isovalue) {
                                pos.push_back(glm::vec3 { x, y, z });
                                continue;
                            }
                        }
                        if (y - 1 >= 0) {
                            if (data.value(x, y - 1, z) <= isovalue) {
                                pos.push_back(glm::vec3 { x, y, z });
                                continue;
                            }
                        }
                        if (z + 1 < reso.z) {
                            if (data.value(x, y, z + 1) <= isovalue) {
                                pos.push_back(glm::vec3 { x, y, z });
                                continue;
                            }
                        }
                        if (z - 1 >= 0) {
                            if (data.value(x, y, z - 1) <= isovalue) {
                                pos.push_back(glm::vec3 { x, y, z });
                                continue;
                            }
                        }
                    }
                }
            }
        }

        texcoord = std::vector<glm::vec2>(pos.size(), glm::vec2(0.0f, 0.0f));
        renderer_->LoadVolumetricModel();
        Logger::info("%lu voxels will be rendered.", pos.size());
    }

    const float FLOAT_MAX = std::numeric_limits<float>::max();
    glm::vec3 center;
    glm::vec3 min = { FLOAT_MAX, FLOAT_MAX, FLOAT_MAX };
    glm::vec3 max = { -FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX };

    if (world.polyModel != nullptr) {
        posData = world.polyModel->positions;
    } else if (world.volModel != nullptr) {
        posData = world.volModel->positions;
    }

    for (auto const& p : posData) {
        if (p.x > max.x) {
            max.x = p.x;
        }
        if (p.y > max.y) {
            max.y = p.y;
        }
        if (p.z > max.z) {
            max.z = p.z;
        }
        if (p.x < min.x) {
            min.x = p.x;
        }
        if (p.y < min.y) {
            min.y = p.y;
        }
        if (p.z < min.z) {
            min.z = p.z;
        }
    }
    center = (max + min) * 0.5f;

    Logger::info("Camera looking at: %s", glm::to_string(center).c_str());
    world.dataset = std::make_unique<InputData>(posData);
    renderer_->GetCamera().SetCenter(center.x, center.y, center.z);
    renderer_->LoadLattice();

    float maxWidth = max.x;
    if (maxWidth < max.y) {
        maxWidth = max.y;
    }
    if (maxWidth < max.z) {
        maxWidth = max.z;
    }
    renderer_->GetCamera().SetViewVolumeWidth(maxWidth);
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

void OpenGLCanvas::ResetLattice()
{
    BuildLatticeMesh();
    UpdateLatticeEdges();
    renderer_->LoadLattice();
}

void OpenGLCanvas::SetModelColorAlpha(float alpha)
{
    // The range between 0.0 and 1.0 should be handled by UI
    world.modelColorAlpha = alpha;
}

float OpenGLCanvas::GetModelTransparency() const
{
    return world.modelColorAlpha;
}

void OpenGLCanvas::UpdateScene()
{
    if (rendopt & RenderOption_LatticeVertex || rendopt & RenderOption_LatticeEdge
        || rendopt & RenderOption_LatticeFace) {
        BuildLatticeMesh();
    }
}

void OpenGLCanvas::UpdateLatticeEdges()
{
    std::vector<unsigned int> indices;

    int const width = world.lattice->Width();
    int const height = world.lattice->Height();
    for (int i = 0; i < height - 1; ++i) {
        for (int j = 0; j < width - 1; ++j) {
            indices.push_back(i * width + j);
            indices.push_back(i * width + j + 1);
            indices.push_back(i * width + j + 1);
            indices.push_back((i + 1) * width + j + 1);
            indices.push_back((i + 1) * width + j + 1);
            indices.push_back((i + 1) * width + j);
            indices.push_back((i + 1) * width + j);
            indices.push_back(i * width + j);
        }
    }

    world.latticeEdges = indices;
}

void OpenGLCanvas::BuildLatticeMesh()
{
    if (world.lattice->IsTrainingDone()) {
        return;
    }

    Mesh mesh;

    std::vector<glm::vec3> positions;

    // Positions
    auto const& neurons = world.lattice->Neurons();
    positions.reserve(neurons.size());
    for (Node const& n : neurons) {
        positions.emplace_back(n[0], n[1], n[2]);
    }

    int const width = world.lattice->Width();
    int const height = world.lattice->Height();
    float const divisor = 1.1f; // FIXME

    for (int y = 0; y < height - 1; ++y) {
        for (int x = 0; x < width - 1; ++x) {

            unsigned int const idx = y * width + x;
            glm::vec3 p1, p2, p3, p4;
            glm::vec3 n1, n2, n3, n4;
            glm::vec2 t1, t2, t3, t4;
            Face f1, f2;

            /**
             *  4-----3
             *  |     |
             *  |     |
             *  1-----2
             */
            p1 = positions[idx]; // [x, y]
            p2 = positions[idx + 1]; // [x + 1, y]
            p3 = positions[idx + width + 1]; // [x + 1, y + 1]
            p4 = positions[idx + width]; // [x, y + 1]

            // Normals
            n2 = glm::normalize(glm::cross(p2 - p1, p3 - p2));
            n4 = glm::normalize(glm::cross(p3 - p1, p4 - p3));

            if (isnan(n1.x) || isnan(n1.y) || isnan(n1.z)) {
                n1 = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            if (isnan(n2.x) || isnan(n2.y) || isnan(n2.z)) {
                n2 = glm::vec3(0.0f, 0.0f, 0.0f);
            }

            n3 = (n2 + n4) * 0.5f;
            n1 = (n2 + n4) * 0.5f;

            // TextureCoords
            t1 = glm::vec2(x / divisor, y / divisor);
            t2 = glm::vec2((x + 1) / divisor, y / divisor);
            t3 = glm::vec2((x + 1) / divisor, (y + 1) / divisor);
            t4 = glm::vec2(x / divisor, (y + 1) / divisor);

            mesh.positions.push_back(p1);
            mesh.positions.push_back(p2);
            mesh.positions.push_back(p3);
            mesh.positions.push_back(p1);
            mesh.positions.push_back(p3);
            mesh.positions.push_back(p4);
            mesh.normals.push_back(n1);
            mesh.normals.push_back(n2);
            mesh.normals.push_back(n3);
            mesh.normals.push_back(n1);
            mesh.normals.push_back(n3);
            mesh.normals.push_back(n4);
            mesh.textureCoords.push_back(t1);
            mesh.textureCoords.push_back(t2);
            mesh.textureCoords.push_back(t3);
            mesh.textureCoords.push_back(t1);
            mesh.textureCoords.push_back(t3);
            mesh.textureCoords.push_back(t4);
        }
    }

    world.neurons.positions = positions;
    world.latticeMesh = mesh;
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

wxBEGIN_EVENT_TABLE(OpenGLCanvas, wxGLCanvas)
    EVT_PAINT(OpenGLCanvas::OnPaint)
    EVT_SIZE(OpenGLCanvas::OnSize)
    EVT_MOTION(OpenGLCanvas::OnMouseMotion)
    EVT_LEFT_DOWN(OpenGLCanvas::OnMouseLeftDown)
    EVT_RIGHT_DOWN(OpenGLCanvas::OnMouseRightDown)
    EVT_MOUSEWHEEL(OpenGLCanvas::OnMouseWheel)
wxEND_EVENT_TABLE()
