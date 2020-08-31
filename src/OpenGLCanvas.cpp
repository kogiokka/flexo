#include "OpenGLCanvas.hpp"

#include <fstream>
#include <iostream>

OpenGLCanvas::OpenGLCanvas(wxWindow* parent,
                           wxGLAttributes const& dispAttrs,
                           wxWindowID id,
                           wxPoint const& pos,
                           wxSize const& size,
                           long style,
                           wxString const& name)
  : wxGLCanvas(parent, dispAttrs, id, pos, size, style, name)
  , isGLLoaded_(false)
  , toTrain_(false)
  , vboSurf_(0)
  , vboPosModel_(0)
  , vboLatPos_(0)
  , iboLatLines_(0)
  , iboLatSurf_(0)
  , surfaceTransparency_(0.8f)
  , iterPerFrame_(10)
  , random_(nullptr)
  , vao_(nullptr)
  , context_(nullptr)
  , shader_(nullptr)
  , shaderLines_(nullptr)
  , shaderNodes_(nullptr)
  , camera_(nullptr)
  , lattice_(nullptr)
  , linesIdx_(0)
  , surfsIdx_(0)
{
  wxGLContextAttrs attrs;
  attrs.CoreProfile().OGLVersion(4, 5).Robust().EndList();
  context_ = new wxGLContext(this, nullptr, &attrs);

  lattice_ = new Lattice(64, 50000, 0.15f);
  wxSize clientSize = GetClientSize() * GetContentScaleFactor();
  camera_ = new Camera(clientSize.x, clientSize.y);

  renderOpt_ = {true, true, true, false};
};

OpenGLCanvas::~OpenGLCanvas()
{
  delete random_;
  delete vao_;
  delete context_;
  delete shader_;
  delete shaderLines_;
  delete shaderNodes_;
  delete camera_;
  delete lattice_;
  delete surface_;
  delete posModel_;
}

void
OpenGLCanvas::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);
  SetCurrent(*context_);

  static float s_scale = 50.0f;
  static auto const s_scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.4f);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto const& neurons = lattice_->neurons();
  std::vector<std::array<float, 3>> renderPos;
  renderPos.reserve(neurons.size());
  for (auto const& n : neurons) {
    float const x = n[0] * s_scale;
    float const y = n[1] * s_scale;
    float const z = n[2] * s_scale;
    renderPos.push_back({x, y, z});
  }
  glNamedBufferSubData(vboLatPos_, 0, renderPos.size() * 3 * sizeof(float), renderPos.data());

  if (renderOpt_[LAT_VERTEX]) {
    vao_->enable("instanced");
    shaderNodes_->Use();
    shaderNodes_->SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
    shaderNodes_->SetUniform3fv("viewPos", camera_->Position());
    shaderNodes_->SetUniform3fv("lightSrc", camera_->Position());
    shaderNodes_->SetUniformMatrix4fv("viewProjMat", camera_->ViewProjectionMatrix());
    shaderNodes_->SetUniformMatrix4fv("modelMat", s_scaleMat);
    glVertexArrayVertexBuffer(vao_->id(), 0, vboPosModel_, 0, posModel_->stride());
    glVertexArrayVertexBuffer(vao_->id(), 1, vboPosModel_, 3 * sizeof(float), posModel_->stride());
    glDrawArraysInstanced(GL_TRIANGLES, 0, posModel_->drawArraysCount(), renderPos.size());
    vao_->disable("instanced");
  }

  if (renderOpt_[LAT_FACE]) {
    vao_->disable("normal");
    shaderLines_->Use();
    shaderLines_->SetUniformMatrix4fv("viewProjMat", camera_->ViewProjectionMatrix());
    shaderLines_->SetUniformMatrix4fv("modelMat", glm::mat4(1.0f));
    shaderLines_->SetUniform3f("color", 0.7f, 0.7f, 0.7f);
    glVertexArrayVertexBuffer(vao_->id(), 0, vboLatPos_, 0, 3 * sizeof(float));
    glVertexArrayElementBuffer(vao_->id(), iboLatSurf_);
    glDrawElements(GL_TRIANGLES, surfsIdx_.size(), GL_UNSIGNED_INT, 0);
    vao_->enable("normal");
  }

  if (renderOpt_[LAT_EDGE]) {
    vao_->disable("normal");
    shaderLines_->Use();
    shaderLines_->SetUniformMatrix4fv("viewProjMat", camera_->ViewProjectionMatrix());
    shaderLines_->SetUniformMatrix4fv("modelMat", glm::mat4(1.0f));
    shaderLines_->SetUniform3f("color", 1.0f, 1.0f, 1.0f);
    glVertexArrayVertexBuffer(vao_->id(), 0, vboLatPos_, 0, 3 * sizeof(float));
    glVertexArrayElementBuffer(vao_->id(), iboLatLines_);
    glDrawElements(GL_LINES, linesIdx_.size(), GL_UNSIGNED_INT, 0);
    vao_->enable("normal");
  }

  if (renderOpt_[SURFACE]) {
    shader_->Use();
    shader_->SetUniformMatrix4fv("viewProjMat", camera_->ViewProjectionMatrix());
    shader_->SetUniformMatrix4fv("modelMat", glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * s_scale));
    shader_->SetUniform3fv("viewPos", camera_->Position());
    shader_->SetUniform3fv("lightSrc", camera_->Position());
    shader_->SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
    shader_->SetUniform1f("alpha", surfaceTransparency_);
    glVertexArrayVertexBuffer(vao_->id(), 0, vboSurf_, 0, surface_->stride());
    glVertexArrayVertexBuffer(vao_->id(), 1, vboSurf_, 3 * sizeof(float), surface_->stride());
    glDrawArrays(GL_TRIANGLES, 0, surface_->drawArraysCount());
  }

  for (int i = 0; i < iterPerFrame_; ++i) {
    if (toTrain_) {
      toTrain_ = lattice_->input(surface_->v()[random_->get()]);
    }
  }

  SwapBuffers();
  Refresh();
}

void
OpenGLCanvas::InitGL()
{
  SetCurrent(*context_);

  isGLLoaded_ = gladLoadGL();

  assert(isGLLoaded_);

#ifndef NDEBUG
  glDebugMessageCallback(
    [](GLenum source,
       GLenum type,
       GLuint id,
       GLenum severity,
       GLsizei length,
       GLchar const* message,
       void const* userParam) noexcept {
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
  vao_ = new VertexArray();
  AttribFormat format;
  format.count = 3;
  format.type = GL_FLOAT;
  format.normalized = GL_FALSE;

  vao_->addAttrib("position", 0, format);
  vao_->addAttrib("normal", 1, format);
  vao_->addAttrib("instanced", 2, format);

  surface_ = new ObjModel();
  surface_->read("res/models/NurbsSurface.obj");
  surface_->genVertexBuffer(OBJ_V | OBJ_VN);
  random_ = new RandomIntNumber<unsigned int>(0, surface_->v().size() - 1);
  glCreateBuffers(1, &vboSurf_);
  glNamedBufferStorage(
    vboSurf_, surface_->vertexBuffer().size() * sizeof(float), surface_->vertexBuffer().data(), GL_DYNAMIC_STORAGE_BIT);
  /** Surface END **********************************************************/

  /** Lattice **********************************************************/
  posModel_ = new ObjModel();
  posModel_->read("res/models/Icosphere.obj");
  posModel_->genVertexBuffer(OBJ_V | OBJ_VN);
  glCreateBuffers(1, &vboPosModel_);
  glNamedBufferStorage(vboPosModel_,
                       posModel_->vertexBuffer().size() * sizeof(float),
                       posModel_->vertexBuffer().data(),
                       GL_DYNAMIC_STORAGE_BIT);

  linesIdx_ = lattice_->lineIndices();
  surfsIdx_ = lattice_->triangleIndices();

  glCreateBuffers(1, &vboLatPos_);
  glCreateBuffers(1, &iboLatLines_);
  glCreateBuffers(1, &iboLatSurf_);
  glNamedBufferStorage(vboLatPos_, lattice_->neurons().size() * 3 * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferStorage(iboLatLines_, linesIdx_.size() * sizeof(unsigned int), linesIdx_.data(), GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferStorage(iboLatSurf_, surfsIdx_.size() * sizeof(unsigned int), surfsIdx_.data(), GL_DYNAMIC_STORAGE_BIT);

  shaderLines_ = new Shader();
  shaderLines_->Attach(GL_VERTEX_SHADER, "shader/lines.vert");
  shaderLines_->Attach(GL_FRAGMENT_SHADER, "shader/lines.frag");
  shaderLines_->Link();

  shaderNodes_ = new Shader();
  shaderNodes_->Attach(GL_VERTEX_SHADER, "shader/nodes.vert");
  shaderNodes_->Attach(GL_FRAGMENT_SHADER, "shader/nodes.frag");
  shaderNodes_->Link();
  /** Lattice END ******************************************************/

  glVertexArrayVertexBuffer(vao_->id(), 2, vboLatPos_, 0, 3 * sizeof(float));
  glVertexArrayBindingDivisor(vao_->id(), 2, 1);

  vao_->enable("position");
  vao_->enable("normal");
  vao_->bind();

  shader_ = new Shader();
  shader_->Attach(GL_VERTEX_SHADER, "shader/default.vert");
  shader_->Attach(GL_FRAGMENT_SHADER, "shader/default.frag");
  shader_->Link();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  std::cout << "Version:      " << glGetString(GL_VERSION) << "\n"
            << "Renderer:     " << glGetString(GL_RENDERER) << "\n"
            << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
            << "Vendor:       " << glGetString(GL_VENDOR) << std::endl;
}

void
OpenGLCanvas::OnSize(wxSizeEvent& event)
{
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
  camera_->WheelZoom(-event.GetWheelRotation() * 0.05f);
}

void
OpenGLCanvas::OnMouseLeftDown(wxMouseEvent& event)
{
  wxCoord const x = event.GetX();
  wxCoord const y = event.GetY();
  camera_->InitDragTranslation(x, y);
}

void
OpenGLCanvas::OnMouseRightDown(wxMouseEvent& event)
{
  wxCoord const x = event.GetX();
  wxCoord const y = event.GetY();
  camera_->InitDragRotation(x, y);
}

void
OpenGLCanvas::OnMouseMotion(wxMouseEvent& event)
{
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
  delete camera_;

  wxSize const size = GetClientSize() * GetContentScaleFactor();
  camera_ = new Camera(size.x, size.y);
}

void
OpenGLCanvas::OpenSurface(const std::string& path)
{
  delete surface_;
  surface_ = new ObjModel();
  surface_->read(path);
  surface_->genVertexBuffer(OBJ_V | OBJ_VN);
  random_ = new RandomIntNumber<unsigned int>(0, surface_->v().size() - 1);
  glDeleteBuffers(1, &vboSurf_);
  glCreateBuffers(1, &vboSurf_);
  glNamedBufferStorage(
    vboSurf_, surface_->vertexBuffer().size() * sizeof(float), surface_->vertexBuffer().data(), GL_DYNAMIC_STORAGE_BIT);
}

void
OpenGLCanvas::SetPlayOrPause(bool toTrain)
{
  toTrain_ = toTrain;
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
  return lattice_->currentIteration();
}

void
OpenGLCanvas::ResetLattice(int iterationCap, float initLearningRate, int dimension)
{
  toTrain_ = false;
  bool const isSameIter = (iterationCap == lattice_->maxIterations());
  bool const isSameRate = (initLearningRate == lattice_->initialRate());
  bool const isSameDimen = (dimension == lattice_->dimension());
  bool const changed = !(isSameIter && isSameDimen && isSameRate);
  delete lattice_;
  lattice_ = new Lattice(dimension, iterationCap, initLearningRate);

  if (changed) {
    linesIdx_ = lattice_->lineIndices();
    surfsIdx_ = lattice_->triangleIndices();

    glDeleteBuffers(1, &iboLatLines_);
    glDeleteBuffers(1, &iboLatSurf_);

    glCreateBuffers(1, &iboLatLines_);
    glCreateBuffers(1, &iboLatSurf_);
    glNamedBufferStorage(
      iboLatLines_, linesIdx_.size() * sizeof(unsigned int), linesIdx_.data(), GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(
      iboLatSurf_, surfsIdx_.size() * sizeof(unsigned int), surfsIdx_.data(), GL_DYNAMIC_STORAGE_BIT);

    glDeleteBuffers(1, &vboLatPos_);
    glCreateBuffers(1, &vboLatPos_);
    glNamedBufferStorage(vboLatPos_, lattice_->neurons().size() * 3 * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT);
    glVertexArrayVertexBuffer(vao_->id(), 2, vboLatPos_, 0, 3 * sizeof(float));
  }
}

int
OpenGLCanvas::GetIterationCap() const
{
  return lattice_->maxIterations();
}

float
OpenGLCanvas::GetInitialLearningRate() const
{
  return lattice_->initialRate();
}

int
OpenGLCanvas::GetLatticeDimension() const
{
  return lattice_->dimension();
}

void
OpenGLCanvas::SetSurfaceTransparency(float alpha)
{
  // The range between 0.0 and 1.0 should be handled by UI
  surfaceTransparency_ = alpha;
}

float
OpenGLCanvas::GetSurfaceTransparency() const
{
  return surfaceTransparency_;
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
