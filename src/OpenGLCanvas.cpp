#include "OpenGLCanvas.hpp"
#include "assetlib/OBJ/OBJImporter.hpp"
#include "assetlib/STL/STLImporter.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>

constexpr std::size_t static SIZE_FLOAT = sizeof(float);
constexpr std::size_t static SIZE_UINT = sizeof(unsigned int);

OpenGLCanvas::OpenGLCanvas(wxWindow* parent,
                           wxGLAttributes const& dispAttrs,
                           wxWindowID id,
                           wxPoint const& pos,
                           wxSize const& size,
                           long style,
                           wxString const& name)
  : wxGLCanvas(parent, dispAttrs, id, pos, size, style, name)
  , isGLLoaded_(false)
  , isAcceptingInput_(false)
  , vboSurf_(0)
  , vboLatFace_(0)
  , vboLatPos_(0)
  , iboLatEdge_(0)
  , vboVertModel_(0)
  , surfaceColorAlpha_(0.8f)
  , iterPerFrame_(10)
  , RNG_(nullptr)
  , vao_(nullptr)
  , context_(nullptr)
  , shader_(nullptr)
  , shaderEdge_(nullptr)
  , shaderVertexModel_(nullptr)
  , camera_(nullptr)
  , lattice_(nullptr)
  , latEdgeIndices_(0)
  , latFaceIndices_(0)
{
  wxGLContextAttrs attrs;
  attrs.CoreProfile().OGLVersion(4, 3).Robust().EndList();
  context_ = std::make_unique<wxGLContext>(this, nullptr, &attrs);

  lattice_ = std::make_unique<Lattice>(64, 64, 50000, 0.15f);
  wxSize clientSize = GetClientSize() * GetContentScaleFactor();
  camera_ = std::make_unique<Camera>(clientSize.x, clientSize.y);

  renderOpt_ = {true, true, true, false};
}

OpenGLCanvas::~OpenGLCanvas() {}

void
OpenGLCanvas::OnPaint(wxPaintEvent&)
{
  wxPaintDC dc(this);
  SetCurrent(*context_);

  static float s_scale = 30.0f;
  static auto const s_scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.2f);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto const& neurons = lattice_->Neurons();
  std::vector<std::array<float, 3>> latPos;
  latPos.reserve(neurons.size());
  for (auto const& n : neurons) {
    float const x = n[0] * s_scale;
    float const y = n[1] * s_scale;
    float const z = n[2] * s_scale;
    latPos.push_back({x, y, z});
  }
  // OpenGL 4.5
  // glNamedBufferSubData(vboLatPos_, 0, neurons.size() * 3 * SIZE_FLOAT, latPos.data());

  glBindBuffer(GL_ARRAY_BUFFER, vboLatPos_);
  // Position(3): 3 * size of float
  glBufferSubData(GL_ARRAY_BUFFER, 0, neurons.size() * 3 * SIZE_FLOAT, latPos.data());

  if (renderOpt_[LAT_VERTEX]) {
    vao_->Enable("normal");
    vao_->Enable("instanced");
    shaderVertexModel_->Use();
    shaderVertexModel_->SetUniformMatrix4fv("viewProjMat", camera_->ViewProjectionMatrix());
    shaderVertexModel_->SetUniformMatrix4fv("modelMat", s_scaleMat);
    shaderVertexModel_->SetUniform3fv("viewPos", camera_->Position());
    shaderVertexModel_->SetUniform3fv("light.position", camera_->Position());
    shaderVertexModel_->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
    shaderVertexModel_->SetUniform3f("light.diffusion", 0.5f, 0.5f, 0.5f);
    shaderVertexModel_->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);
    shaderVertexModel_->SetUniform3f("material.ambient", 1.0f, 1.0f, 1.0f);
    shaderVertexModel_->SetUniform3f("material.diffusion", 1.0f, 1.0f, 1.0f);
    shaderVertexModel_->SetUniform3f("material.specular", 0.3f, 0.3f, 0.3f);
    shaderVertexModel_->SetUniform1f("material.shininess", 32.0f);
    shaderVertexModel_->SetUniform1f("alpha", 1.0f);
    // OpenGL 4.5
    // glVertexArrayVertexBuffer(vao_->Id(), 0, vboVertModel_, 0, vertModel_->Stride());
    // glVertexArrayVertexBuffer(vao_->Id(), 1, vboVertModel_, 3 * SIZE_FLOAT, vertModel_->Stride());
    // glDrawArraysInstanced(GL_TRIANGLES, 0, vertModel_->DrawArraysCount(), latPos.size());
    glBindVertexBuffer(0, vboVertModel_, offsetof(Vertex, position), sizeof(Vertex));
    glBindVertexBuffer(1, vboVertModel_, offsetof(Vertex, normal), sizeof(Vertex));
    glDrawArraysInstanced(GL_TRIANGLES, 0, vertModel_.Vertices().size() * 3, latPos.size());
  }

  if (renderOpt_[LAT_FACE]) {
    std::vector<glm::vec3> latFace;
    std::size_t drawArraysCount = 0;
    latFace.reserve(latFaceIndices_.size());
    for (std::size_t i = 0; i < latFaceIndices_.size(); i += 3) {
      auto const idx1 = latFaceIndices_[i];
      auto const idx2 = latFaceIndices_[i + 1];
      auto const idx3 = latFaceIndices_[i + 2];
      auto const p1 = glm::vec3{latPos[idx1][0], latPos[idx1][1], latPos[idx1][2]};
      auto const p2 = glm::vec3{latPos[idx2][0], latPos[idx2][1], latPos[idx2][2]};
      auto const p3 = glm::vec3{latPos[idx3][0], latPos[idx3][1], latPos[idx3][2]};

      glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
      if (std::isnan(normal.x) || std::isnan(normal.y) || std::isnan(normal.z)) {
        normal = glm::vec3(0.0f, 0.0f, 0.0f);
      }
      latFace.push_back(p1);
      latFace.push_back(normal);
      latFace.push_back(p2);
      latFace.push_back(normal);
      latFace.push_back(p3);
      latFace.push_back(normal);
      drawArraysCount += 3; // Added 3 vertices
    }

    // OpenGL 4.5
    // glNamedBufferSubData(vboLatFace_, 0, latFaceIndices_.size() * 6 * SIZE_FLOAT, latFace.data());

    glBindBuffer(GL_ARRAY_BUFFER, vboLatFace_);
    // Position(3) + Normal(3): 6 * size of float
    glBufferSubData(GL_ARRAY_BUFFER, 0, latFaceIndices_.size() * 6 * SIZE_FLOAT, latFace.data());

    vao_->Enable("normal");
    vao_->Disable("instanced");
    shader_->Use();
    shader_->SetUniformMatrix4fv("viewProjMat", camera_->ViewProjectionMatrix());
    shader_->SetUniformMatrix4fv("modelMat", glm::mat4(1.0f));
    shader_->SetUniform3fv("viewPos", camera_->Position());
    shader_->SetUniform3fv("light.position", camera_->Position());
    shader_->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
    shader_->SetUniform3f("light.diffusion", 0.5f, 0.5f, 0.5f);
    shader_->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);
    shader_->SetUniform3f("material.ambient", 1.0f, 1.0f, 1.0f);
    shader_->SetUniform3f("material.diffusion", 1.0f, 1.0f, 1.0f);
    shader_->SetUniform3f("material.specular", 0.3f, 0.3f, 0.3f);
    shader_->SetUniform1f("material.shininess", 32.0f);
    shader_->SetUniform1f("alhpa", 1.0f);
    // OpenGL 4.5
    // glVertexArrayVertexBuffer(vao_->Id(), 0, vboLatFace_, 0, 6 * SIZE_FLOAT);
    // glVertexArrayVertexBuffer(vao_->Id(), 1, vboLatFace_, 3 * SIZE_FLOAT, 6 * SIZE_FLOAT);
    // glDrawArrays(GL_TRIANGLES, 0, drawArraysCount);
    glBindVertexBuffer(0, vboLatFace_, 0, 6 * SIZE_FLOAT);
    glBindVertexBuffer(1, vboLatFace_, 3 * SIZE_FLOAT, 6 * SIZE_FLOAT);
    glDrawArrays(GL_TRIANGLES, 0, drawArraysCount);
  }

  if (renderOpt_[LAT_EDGE]) {
    vao_->Disable("normal");
    vao_->Disable("instanced");
    shaderEdge_->Use();
    shaderEdge_->SetUniformMatrix4fv("viewProjMat", camera_->ViewProjectionMatrix());
    shaderEdge_->SetUniformMatrix4fv("modelMat", glm::mat4(1.0f));
    shaderEdge_->SetUniform3f("color", 1.0f, 1.0f, 1.0f);
    // OpenGL 4.5
    // glVertexArrayVertexBuffer(vao_->Id(), 0, vboLatPos_, 0, 3 * SIZE_FLOAT);
    // glVertexArrayElementBuffer(vao_->Id(), iboLatEdge_);
    // glDrawElements(GL_LINES, latEdgeIndices_.size(), GL_UNSIGNED_INT, 0);
    glBindVertexBuffer(0, vboLatPos_, 0, 3 * SIZE_FLOAT);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboLatEdge_);
    glDrawElements(GL_LINES, latEdgeIndices_.size(), GL_UNSIGNED_INT, 0);
  }

  if (renderOpt_[SURFACE]) {
    vao_->Enable("normal");
    vao_->Disable("instanced");
    shader_->Use();
    shader_->SetUniformMatrix4fv("viewProjMat", camera_->ViewProjectionMatrix());
    shader_->SetUniformMatrix4fv("modelMat", glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * s_scale));
    shader_->SetUniform3fv("viewPos", camera_->Position());
    shader_->SetUniform3fv("light.position", camera_->Position());
    shader_->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
    shader_->SetUniform3f("light.diffusion", 0.5f, 0.5f, 0.5f);
    shader_->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);
    shader_->SetUniform3f("material.ambient", 1.0f, 1.0f, 1.0f);
    shader_->SetUniform3f("material.diffusion", 0.67f, 0.8f, 1.0f);
    shader_->SetUniform3f("material.specular", 0.3f, 0.3f, 0.3f);
    shader_->SetUniform1f("material.shininess", 32.0f);
    shader_->SetUniform1f("alpha", surfaceColorAlpha_);
    // OpenGL 4.5
    // glVertexArrayVertexBuffer(vao_->Id(), 0, vboSurf_, 0, surface_->Stride());
    // glVertexArrayVertexBuffer(vao_->Id(), 1, vboSurf_, 3 * SIZE_FLOAT, surface_->Stride());
    // glDrawArrays(GL_TRIANGLES, 0, surface_->DrawArraysCount());
    glBindVertexBuffer(0, vboSurf_, offsetof(Vertex, position), sizeof(Vertex));
    glBindVertexBuffer(1, vboSurf_, offsetof(Vertex, position), sizeof(Vertex));
    glDrawArrays(GL_TRIANGLES, 0, surface_.Vertices().size());
  }

  for (int i = 0; i < iterPerFrame_; ++i) {
    if (isAcceptingInput_) {
      auto const& [x, y, z] = surface_.Vertices()[RNG_->scalar()].position;
      isAcceptingInput_ = lattice_->Input(std::vector{x, y, z});
    }
  }

  SwapBuffers();
}

void
OpenGLCanvas::InitGL()
{
  SetCurrent(*context_);

  isGLLoaded_ = gladLoadGL();

  assert(isGLLoaded_);

#ifndef NDEBUG
  glDebugMessageCallback(
    []([[maybe_unused]] GLenum source,
       [[maybe_unused]] GLenum type,
       [[maybe_unused]] GLuint id,
       [[maybe_unused]] GLenum severity,
       [[maybe_unused]] GLsizei length,
       [[maybe_unused]] GLchar const* message,
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

  /** Surface **************************************************************/
  vao_ = std::make_unique<VertexArray>();
  AttribFormat format;
  format.count = 3;
  format.type = GL_FLOAT;
  format.normalized = GL_FALSE;

  // We are using glVertexAttribFormat from OpenGL 4.3, execute glBindVertexArray first before adding new attributes.
  vao_->Bind();
  vao_->AddAttribFormat("position", 0, format);
  vao_->AddAttribFormat("normal", 1, format);
  vao_->AddAttribFormat("instanced", 2, format);

  OBJImporter obj;
  obj.Read("res/models/surface2.obj");
  surface_.Import(obj.Model());
  RNG_ = std::make_unique<RandomIntNumber<unsigned int>>(0, surface_.Vertices().size() - 1);

  glGenBuffers(1, &vboSurf_);
  glBindBuffer(GL_ARRAY_BUFFER, vboSurf_);
  glBufferData(
    GL_ARRAY_BUFFER, surface_.Vertices().size() * sizeof(Vertex), surface_.Vertices().data(), GL_STATIC_DRAW);
  /** Surface END **********************************************************/

  /** Lattice **********************************************************/

  STLImporter stl;
  stl.Read("res/models/UVSphere.stl");
  vertModel_.Import(stl.Model());
  glGenBuffers(1, &vboVertModel_);
  glBindBuffer(GL_ARRAY_BUFFER, vboVertModel_);
  glBufferData(
    GL_ARRAY_BUFFER, vertModel_.Vertices().size() * sizeof(Vertex), vertModel_.Vertices().data(), GL_STATIC_DRAW);

  latEdgeIndices_ = lattice_->EdgeIndices();
  latFaceIndices_ = lattice_->FaceIndices();
  glGenBuffers(1, &vboLatFace_);
  glBindBuffer(GL_ARRAY_BUFFER, vboLatFace_);
  // GL_DYNAMIC_DRAW: The data store contents will be modified repeatedly and used many times.
  glBufferData(GL_ARRAY_BUFFER, latFaceIndices_.size() * 6 * SIZE_FLOAT, nullptr, GL_DYNAMIC_DRAW);

  // An Vertex Buffer Object storing the positions of neurons on the lattice.
  glGenBuffers(1, &vboLatPos_);
  glBindBuffer(GL_ARRAY_BUFFER, vboLatPos_);
  // GL_DYNAMIC_DRAW: The data store contents will be modified repeatedly and used many times.
  glBufferData(GL_ARRAY_BUFFER, lattice_->Neurons().size() * 3 * SIZE_FLOAT, nullptr, GL_DYNAMIC_DRAW);

  // An Index Buffer that holds indices referencing the positions of neurons to draw the edges between them.
  glGenBuffers(1, &iboLatEdge_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboLatEdge_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, latEdgeIndices_.size() * SIZE_UINT, latEdgeIndices_.data(), GL_STATIC_DRAW);
  /** Lattice END ******************************************************/

  // If we use OpenGL 4.5 functions:
  // glCreateBuffers(1, &vboSurf_);
  // glNamedBufferStorage(
  //   vboSurf_, surface_->VertexBuffer().size() * SIZE_FLOAT, surface_->VertexBuffer().data(), GL_DYNAMIC_STORAGE_BIT);
  // glCreateBuffers(1, &vboVertModel_);
  // glNamedBufferStorage(vboVertModel_, vertModel_->VertexBuffer().size() * SIZE_FLOAT,
  //   vertModel_->VertexBuffer().data(), GL_DYNAMIC_STORAGE_BIT); glCreateBuffers(1, &vboLatFace_);
  // glNamedBufferStorage(vboLatFace_, latFaceIndices_.size() * 6 * SIZE_FLOAT, nullptr, GL_DYNAMIC_STORAGE_BIT);
  // glCreateBuffers(1, &vboLatPos_);
  // glNamedBufferStorage(vboLatPos_, lattice_->Neurons().size() * 3 * SIZE_FLOAT, nullptr, GL_DYNAMIC_STORAGE_BIT);
  // glCreateBuffers(1, &iboLatEdge_);
  // glNamedBufferStorage(
  //   iboLatEdge_, latEdgeIndices_.size() * SIZE_UINT, latEdgeIndices_.data(), GL_DYNAMIC_STORAGE_BIT);

  shaderEdge_ = std::make_unique<Shader>();
  shaderEdge_->Attach(GL_VERTEX_SHADER, "shader/Edge.vert");
  shaderEdge_->Attach(GL_FRAGMENT_SHADER, "shader/Edge.frag");
  shaderEdge_->Link();

  /** OpenGL Instancing
   *  Use glDrawArraysInstanced to render tens of thousands of objects that represent the lattice vertices.
   */
  // OpenGL 4.5
  // glVertexArrayVertexBuffer(vao_->Id(), 2, vboLatPos_, 0, 3 * SIZE_FLOAT);
  // glVertexArrayBindingDivisor(vao_->Id(), 2, 1);
  glBindVertexBuffer(2, vboLatPos_, 0, 3 * SIZE_FLOAT);
  glVertexBindingDivisor(2, 1);

  vao_->Enable("position");
  vao_->Enable("normal");

  shader_ = std::make_unique<Shader>();
  shader_->Attach(GL_VERTEX_SHADER, "shader/default.vert");
  shader_->Attach(GL_FRAGMENT_SHADER, "shader/default.frag");
  shader_->Link();

  shaderVertexModel_ = std::make_unique<Shader>();
  shaderVertexModel_->Attach(GL_VERTEX_SHADER, "shader/VertexModel.vert");
  shaderVertexModel_->Attach(GL_FRAGMENT_SHADER, "shader/VertexModel.frag");
  shaderVertexModel_->Link();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  std::cout << "Version:      " << glGetString(GL_VERSION) << "\n"
            << "Renderer:     " << glGetString(GL_RENDERER) << "\n"
            << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
            << "Vendor:       " << glGetString(GL_VENDOR) << std::endl;
}

void
OpenGLCanvas::OnSize(wxSizeEvent&)
{
  assert(camera_.get() != nullptr);
  assert(context_.get() != nullptr);

  // Adjust aspect ratio of the camera before GLCanvas is displayed, in response to any resizing of the canvas.
  wxSize const size = GetClientSize() * GetContentScaleFactor();
  camera_->SetAspectRatio(size.x, size.y);

  // Guard for SetCurrent and calling GL functions
  if (!IsShownOnScreen() || !isGLLoaded_)
    return;

  SetCurrent(*context_);
  glViewport(0, 0, size.x, size.y);
}

void
OpenGLCanvas::OnMouseWheel(wxMouseEvent& event)
{
  assert(camera_.get() != nullptr);
  camera_->WheelZoom(event.GetWheelRotation() / 120);
}

void
OpenGLCanvas::OnMouseLeftDown(wxMouseEvent& event)
{
  assert(camera_.get() != nullptr);
  wxCoord const x = event.GetX();
  wxCoord const y = event.GetY();
  camera_->InitDragTranslation(x, y);
}

void
OpenGLCanvas::OnMouseRightDown(wxMouseEvent& event)
{
  assert(camera_.get() != nullptr);
  wxCoord const x = event.GetX();
  wxCoord const y = event.GetY();
  camera_->InitDragRotation(x, y);
}

void
OpenGLCanvas::OnMouseMotion(wxMouseEvent& event)
{
  assert(camera_.get() != nullptr);
  wxCoord const x = event.GetX();
  wxCoord const y = event.GetY();
  if (event.LeftIsDown()) {
    camera_->DragTranslation(x, y);
  }
  if (event.RightIsDown()) {
    camera_->DragRotation(x, y);
  }
}

void
OpenGLCanvas::ResetCamera()
{
  wxSize const size = GetClientSize() * GetContentScaleFactor();
  camera_ = std::make_unique<Camera>(size.x, size.y);
}

void
OpenGLCanvas::OpenSurface(std::string const& path)
{
  OBJImporter obj;
  obj.Read(path);
  surface_.Import(obj.Model());
  RNG_ = std::make_unique<RandomIntNumber<unsigned int>>(0, surface_.Vertices().size() - 1);
  glDeleteBuffers(1, &vboSurf_);
  // OpenGL 4.5
  // glCreateBuffers(1, &vboSurf_);
  // glNamedBufferStorage(
  //   vboSurf_, surface_->VertexBuffer().size() * SIZE_FLOAT, surface_->VertexBuffer().data(),
  //   GL_DYNAMIC_STORAGE_BIT);
  glGenBuffers(1, &vboSurf_);
  glBindBuffer(GL_ARRAY_BUFFER, vboSurf_);
  glBufferData(
    GL_ARRAY_BUFFER, surface_.Vertices().size() * sizeof(Vertex), surface_.Vertices().data(), GL_STATIC_DRAW);
}

void
OpenGLCanvas::SetPlayOrPause(bool isAcceptingInput)
{
  isAcceptingInput_ = isAcceptingInput;
}

void
OpenGLCanvas::ToggleRenderOption(RenderOpt opt)
{
  renderOpt_[opt] = !renderOpt_[opt];
}

bool
OpenGLCanvas::GetRenderOptionState(RenderOpt opt) const
{
  return renderOpt_[opt];
}

int
OpenGLCanvas::GetCurrentIterations() const
{
  return lattice_->CurrentIteration();
}

void
OpenGLCanvas::ResetLattice(int width, int height, int iterationCap, float initLearningRate)
{
  isAcceptingInput_ = false;
  bool const isSameWidth = (width == lattice_->Width());
  bool const isSameHeight = (height == lattice_->Height());
  bool const isSameIter = (iterationCap == lattice_->IterationCap());
  bool const isSameRate = (initLearningRate == lattice_->InitialRate());
  bool const changed = !(isSameWidth && isSameHeight && isSameIter && isSameRate);
  lattice_ = std::make_unique<Lattice>(width, height, iterationCap, initLearningRate);

  if (changed) {
    latEdgeIndices_ = lattice_->EdgeIndices();
    latFaceIndices_ = lattice_->FaceIndices();

    // OpenGL 4.5
    // glDeleteBuffers(1, &iboLatEdge_);
    // glDeleteBuffers(1, &vboLatFace_);
    // glDeleteBuffers(1, &vboLatPos_);
    // glCreateBuffers(1, &iboLatEdge_);
    // glCreateBuffers(1, &vboLatFace_);
    // glCreateBuffers(1, &vboLatPos_);
    // glNamedBufferStorage(
    //   iboLatEdge_, latEdgeIndices_.size() * SIZE_UINT, latEdgeIndices_.data(), GL_DYNAMIC_STORAGE_BIT);
    // glNamedBufferStorage(vboLatFace_, latFaceIndices_.size() * 6 * SIZE_FLOAT, nullptr, GL_DYNAMIC_STORAGE_BIT);
    // glNamedBufferStorage(vboLatPos_, lattice_->Neurons().size() * 3 * SIZE_FLOAT, nullptr, GL_DYNAMIC_STORAGE_BIT);
    // glVertexArrayVertexBuffer(vao_->Id(), 2, vboLatPos_, 0, 3 * SIZE_FLOAT);

    glDeleteBuffers(1, &iboLatEdge_);
    glDeleteBuffers(1, &vboLatFace_);
    glDeleteBuffers(1, &vboLatPos_);
    glGenBuffers(1, &iboLatEdge_);
    glGenBuffers(1, &vboLatFace_);
    glGenBuffers(1, &vboLatPos_);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboLatEdge_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, latEdgeIndices_.size() * SIZE_UINT, latEdgeIndices_.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vboLatFace_);
    glBufferData(GL_ARRAY_BUFFER, latFaceIndices_.size() * 6 * SIZE_FLOAT, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vboLatPos_);
    glBufferData(GL_ARRAY_BUFFER, lattice_->Neurons().size() * 3 * SIZE_FLOAT, nullptr, GL_DYNAMIC_DRAW);

    glBindVertexBuffer(2, vboLatPos_, 0, 3 * SIZE_FLOAT);
  }
}

int
OpenGLCanvas::GetIterationCap() const
{
  assert(lattice_.get() != nullptr);
  return lattice_->IterationCap();
}

float
OpenGLCanvas::GetInitialLearningRate() const
{
  assert(lattice_.get() != nullptr);
  return lattice_->InitialRate();
}

int
OpenGLCanvas::GetLatticeWidth() const
{
  assert(lattice_.get() != nullptr);
  return lattice_->Width();
}

int
OpenGLCanvas::GetLatticeHeight() const
{
  assert(lattice_.get() != nullptr);
  return lattice_->Height();
}

void
OpenGLCanvas::SetSurfaceColorAlpha(float alpha)
{
  // The range between 0.0 and 1.0 should be handled by UI
  surfaceColorAlpha_ = alpha;
}

float
OpenGLCanvas::GetSurfaceTransparency() const
{
  return surfaceColorAlpha_;
}

void
OpenGLCanvas::SetIterationsPerFrame(int times)
{
  iterPerFrame_ = times;
}

int
OpenGLCanvas::GetIterationsPerFrame() const
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
