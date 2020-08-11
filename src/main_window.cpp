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
  m_lattice = new Lattice(16, 1000, 0.1f);
  m_camera = new Camera();
  m_camera->SetAspectRatio(m_width, m_height);
  m_camera->SetProjection(Camera::Projection::Perspective);
}

MainWindow::~MainWindow()
{
  delete m_model;
  delete m_lattice;
  delete m_camera;
  delete m_shader;
}

void
MainWindow::paintGL()
{
  static float const scale = 20.0f;
  static auto const scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3{0.5f, 0.5f,0.5f});
  glViewport(0, 0, m_width, m_height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_shader->SetUniform3fv("viewPos", m_camera->Position());
  m_shader->SetUniform3fv("lightSrc", m_camera->Position());
  m_shader->SetUniformMatrix4fv("viewProjMat", m_camera->ViewProjectionMatrix());
  m_camera->SetCenter(scale / 2, scale / 2, scale / 2);

  for (auto n : m_lattice->neurons()) {
    m_shader->SetUniformMatrix4fv("modelMat", glm::translate(glm::mat4(1.0f), scale * glm::vec3{n[0], n[1], n[2]}) * scaleMat);
    glDrawArrays(GL_TRIANGLES, 0, m_model->vertexCount());
  }

  // ImGui_ImplOpenGL3_NewFrame();
  // ImGui_ImplSDL2_NewFrame(m_window);
  // ImGui::NewFrame();
  // ImGui::Begin("Surface Fitting");
  // ImGui::End();
  // ImGui::Render();
  // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
       void const* userParam) noexcept {
      std::cerr << std::hex;
      std::cerr << "[Type " << type << "]";
      std::cerr << "[Severity " << severity << "]";
      std::cerr << " Message: " << message << "\n";
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

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplOpenGL3_Init();
  ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
}

void
MainWindow::onWindowResized()
{
  m_camera->SetAspectRatio(m_width, m_height);
}

void
MainWindow::onKeyDown(SDL_KeyboardEvent keyEvent)
{
  auto const& io = ImGui::GetIO();
  if (io.WantCaptureKeyboard)
    return;
  switch (keyEvent.keysym.sym) {
  case SDLK_q:
    if (keyEvent.keysym.mod & KMOD_CTRL) {
      m_alive = false;
    }
    break;
  }
}

void
MainWindow::onMouseButtonDown(SDL_MouseButtonEvent buttonEvent)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return;
  switch (buttonEvent.button) {
  case SDL_BUTTON_LEFT:
    m_camera->InitDragTranslation(buttonEvent.x, buttonEvent.y);
    break;
  case SDL_BUTTON_RIGHT:
    m_camera->InitDragRotation(buttonEvent.x, buttonEvent.y);
    break;
  }
}

void
MainWindow::onMouseMotion(SDL_MouseMotionEvent motionEvent)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return;
  switch (motionEvent.state) {
  case SDL_BUTTON_LMASK:
    m_camera->DragTranslation(motionEvent.x, motionEvent.y);
    break;
  case SDL_BUTTON_RMASK:
    m_camera->DragRotation(motionEvent.x, motionEvent.y);
    break;
  }
}

void
MainWindow::onMouseWheel(SDL_MouseWheelEvent wheelEvent)
{
  m_camera->WheelZoom(-wheelEvent.y);
}
