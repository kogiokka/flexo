#include "main_window.hpp"

MainWindow::MainWindow(std::string name, int width, int height)
  : SDLOpenGLWindow(name, width, height)
  , m_vao(0)
  , m_vbo(0)
  , m_vaoLines(0)
  , m_scale(50.0f)
  , m_shader(nullptr)
  , m_shaderLines(nullptr)
  , m_camera(nullptr)
  , m_lattice(nullptr)
  , m_model(nullptr)
{
  m_lattice = new Lattice(16, 1000, 0.1f);
  m_camera = new Camera();
  m_camera->SetAspectRatio(m_width, m_height);
  m_camera->SetProjection(Camera::Projection::Orthographic);
  // Some lighting problem with Perspective mode
  // m_camera->SetProjection(Camera::Projection::Perspective);
  m_camera->SetCenter(m_scale / 2, m_scale / 2, m_scale / 2);
}

MainWindow::~MainWindow()
{
  delete m_model;
  delete m_lattice;
  delete m_camera;
  delete m_shader;
  delete m_shaderLines;
}

void
MainWindow::paintGL()
{
  static auto const scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3{0.5f, 0.5f, 0.5f});
  glViewport(0, 0, m_width, m_height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_shader->Use();
  m_shader->SetUniform3fv("viewPos", m_camera->Position());
  m_shader->SetUniform3fv("lightSrc", m_camera->Position());
  m_shader->SetUniformMatrix4fv("viewProjMat", m_camera->ViewProjectionMatrix());

  auto const neurons = m_lattice->neurons();

  glBindVertexArray(m_vao);
  for (auto const& n : neurons) {
    m_shader->SetUniformMatrix4fv("modelMat",
                                  glm::translate(glm::mat4(1.0f), m_scale * glm::vec3{n[0], n[1], n[2]}) * scaleMat);
    glDrawArrays(GL_TRIANGLES, 0, m_model->vertexCount());
  }

  auto const size = m_lattice->dimension();
  std::vector<int> indices;
  std::vector<float> gridLines;

  for (int i = 0; i < size - 1; ++i) {
    for (int j = 0; j < size - 1; ++j) {
      indices.push_back(i * size + j);
      indices.push_back(i * size + j + 1);
      indices.push_back(i * size + j + 1);
      indices.push_back((i + 1) * size + j + 1);
      indices.push_back((i + 1) * size + j + 1);
      indices.push_back((i + 1) * size + j);
      indices.push_back((i + 1) * size + j);
      indices.push_back(i * size + j);
    }
  }
  for (int i : indices) {
    std::vector<float> weights = neurons[i].weights();
    for (auto& w : weights) {
      w *= m_scale;
    }
    gridLines.insert(gridLines.begin(), weights.begin(), weights.end());
  }

  m_shaderLines->Use();
  m_shaderLines->SetUniformMatrix4fv("viewProjMat", m_camera->ViewProjectionMatrix());
  m_shaderLines->SetUniformMatrix4fv("modelMat", glm::mat4(1.0f));
  glBindVertexArray(m_vaoLines);
  glNamedBufferData(m_vboLines, gridLines.size() * sizeof(float), gridLines.data(), GL_STATIC_DRAW);
  glDrawArrays(GL_LINES, 0, indices.size());

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

  /** Grid nodes (Icosphere model) ********************************************/
  glCreateVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(m_vao, 1, 3, GL_FLOAT, GL_FALSE, 0);

  m_model = new Model();
  m_model->readOBJ("res/models/Icosphere.obj");
  auto const buffer = m_model->vertexBuffer();
  auto const stride = 3 * sizeof(float);
  glCreateBuffers(1, &m_vbo);
  glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, stride);
  glVertexArrayVertexBuffer(m_vao, 1, m_vbo, m_model->vertexCount() * stride, stride);
  glNamedBufferStorage(m_vbo, buffer.size() * sizeof(float), buffer.data(), GL_DYNAMIC_STORAGE_BIT);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  m_shader = new Shader();
  m_shader->Attach(GL_VERTEX_SHADER, "shader/default.vert");
  m_shader->Attach(GL_FRAGMENT_SHADER, "shader/default.frag");
  m_shader->Link();
  m_shader->Use();
  m_shader->SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
  /** Grid Nodes END **********************************************************/

  /** Grid Lines **********************************************************/
  glCreateVertexArrays(1, &m_vaoLines);
  glBindVertexArray(m_vaoLines);
  glVertexArrayAttribFormat(m_vaoLines, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glCreateBuffers(1, &m_vboLines);
  glVertexArrayVertexBuffer(m_vaoLines, 0, m_vboLines, 0, 3 * sizeof(float));
  glEnableVertexAttribArray(0); // IMPORTANT!

  m_shaderLines = new Shader();
  m_shaderLines->Attach(GL_VERTEX_SHADER, "shader/lines.vert");
  m_shaderLines->Attach(GL_FRAGMENT_SHADER, "shader/lines.frag");
  m_shaderLines->Link();
  /** Grid Lines END ******************************************************/

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
  case SDLK_r:
    m_camera->SetCenter(m_scale / 2, m_scale / 2, m_scale / 2);
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
