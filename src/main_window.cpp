#include "main_window.hpp"

MainWindow::MainWindow(std::string name, int width, int height)
  : SDLOpenGLWindow(name, width, height)
  , m_vao(0)
  , m_vbo(0)
  , m_shader(nullptr)
  , m_camera(nullptr)
  , m_lattice(nullptr)
  , m_model(nullptr)
{
  // m_lattice = new Lattice(32, 1000, 0.1f);
  m_camera = new Camera();
  m_camera->SetAspectRatio(m_width, m_height);
}

MainWindow::~MainWindow()
{
  delete m_model;
  delete m_lattice;
  delete m_camera;
  delete m_shader;
}

void
MainWindow::handleEvent(SDL_Event event)
{
  switch (event.type) {
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_q) {
      if (event.key.keysym.mod & KMOD_CTRL) {
        m_alive = false;
      }
    }
    break;
  case SDL_MOUSEWHEEL:
    m_camera->WheelZoom(-event.wheel.y);
    break;
  case SDL_MOUSEBUTTONDOWN:
    switch (event.button.button) {
    case SDL_BUTTON_LEFT:
      m_camera->InitDragTranslation(event.button.x, event.button.y);
      break;
    }
  case SDL_BUTTON_RIGHT: {
    m_camera->InitDragRotation(event.button.x, event.button.y);
    break;
  } break;
  case SDL_MOUSEMOTION:
    switch (event.motion.state) {
    case SDL_BUTTON_LMASK:
      m_camera->DragTranslation(event.motion.x, event.motion.y);
      break;
    case SDL_BUTTON_RMASK:
      m_camera->DragRotation(event.motion.x, event.motion.y);
      break;
    }
    break;
  }
}

void
MainWindow::paintGL()
{
  glViewport(0, 0, m_width, m_height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_shader->SetUniform3fv("viewPos", m_camera->Position());
  m_shader->SetUniform3fv("lightSrc", m_camera->Position());
  m_shader->SetUniformMatrix4fv("viewProjMat", m_camera->ViewProjectionMatrix());

  glDrawArrays(GL_TRIANGLES, 0, m_model->vertexCount());

  SDL_GL_SwapWindow(m_window);
}

void
MainWindow::initializeGL()
{
#ifndef NDEBUG
  glDebugMessageCallback(
    [](GLenum source,
       GLenum type,
       GLuint id,
       GLenum severity,
       GLsizei length,
       GLchar const* message,
       void const* user_param) noexcept {
      std::cerr << "Type: " << std::hex << type << ", ";
      std::cerr << "Severity: " << std::hex << severity << ", ";
      std::cerr << "Message: " << message << "\n";
    },
    nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

  glCreateVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(m_vao, 1, 3, GL_FLOAT, GL_FALSE, 0);

  m_model = new Model();
  m_model->readOBJ("res/models/Icosphere.obj");

  int const stride = 6 * sizeof(float);

  glCreateBuffers(1, &m_vbo);
  glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, stride);
  glVertexArrayVertexBuffer(m_vao, 1, m_vbo, 3 * sizeof(float), stride);

  auto const& buffer = m_model->vertexBuffer();
  glNamedBufferStorage(m_vbo, buffer.size() * sizeof(float), buffer.data(), GL_DYNAMIC_STORAGE_BIT);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  m_shader = new Shader();
  m_shader->Attach(GL_VERTEX_SHADER, "shader/default.vert");
  m_shader->Attach(GL_FRAGMENT_SHADER, "shader/default.frag");
  m_shader->Link();
  m_shader->Use();

  m_shader->SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);

  glEnable(GL_DEPTH_TEST);
}

void
MainWindow::resizeGL()
{
  SDL_GetWindowSize(m_window, &m_width, &m_height);
}
