#include "main_window.hpp"

MainWindow::MainWindow(std::string name, int width, int height)
  : SDLOpenGLWindow(name, width, height)
  , m_vao(0)
  , m_vbo(0)
  , m_vaoLines(0)
  , m_scale(50.0f)
  , m_showModel(true)
  , m_showPoints(true)
  , m_showLines(true)
  , m_isTraining(false)
  , m_shader(nullptr)
  , m_shaderLines(nullptr)
  , m_camera(nullptr)
  , m_lattice(nullptr)
  , m_model(nullptr)
{
  m_lattice = new Lattice(64, 50000, 0.1f);
  m_camera = new Camera(width, height);
  // Some lighting problem with Perspective mode
  // m_camera->SetProjection(Camera::Projection::Perspective);
}

MainWindow::~MainWindow()
{
  delete m_random;
  delete m_model;
  delete m_lattice;
  delete m_camera;
  delete m_shader;
  delete m_shaderLines;
}

void
MainWindow::paintGL()
{
  static auto const scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.4f);
  static auto s_iterNum = m_lattice->maxIterations();
  static auto s_dimen = m_lattice->dimension();
  static auto s_rate = m_lattice->initialRate();

  glViewport(0, 0, m_width, m_height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto const& neurons = m_lattice->neurons();
  std::vector<std::array<float, 3>> renderPos;
  renderPos.reserve(neurons.size());
  for (auto const& n : neurons) {
    float const x = n[0] * m_scale;
    float const y = n[1] * m_scale;
    float const z = n[2] * m_scale;
    renderPos.push_back({x, y, z});
  }

  m_shader->Use();
  m_shader->SetUniform3fv("viewPos", m_camera->Position());
  m_shader->SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
  m_shader->SetUniform3fv("lightSrc", m_camera->Position());
  m_shader->SetUniformMatrix4fv("viewProjMat", m_camera->ViewProjectionMatrix());
  if (m_showPoints) {
    glBindVertexArray(m_vao);
    m_shader->SetUniform1f("alpha", 1.0f);
    for (auto p : renderPos) {
      m_shader->SetUniformMatrix4fv("modelMat",
                                    glm::translate(glm::mat4(1.0f), glm::vec3{p[0], p[1], p[2]}) * scaleMat);
      glDrawArrays(GL_TRIANGLES, 0, m_model->drawArrayCount());
    }
  }

  if (m_showLines) {
    glBindVertexArray(m_vaoLines);
    m_shaderLines->Use();
    m_shaderLines->SetUniformMatrix4fv("viewProjMat", m_camera->ViewProjectionMatrix());
    m_shaderLines->SetUniformMatrix4fv("modelMat", glm::mat4(1.0f));
    glNamedBufferSubData(m_vboLines, 0, renderPos.size() * 3 * sizeof(float), renderPos.data());
    glDrawElements(GL_LINES, m_latticeIndices.size(), GL_UNSIGNED_SHORT, 0);
  }

  if (m_showModel) {
    glBindVertexArray(m_vao);
    m_shader->Use();
    m_shader->SetUniform1f("alpha", 0.6f);
    m_shader->SetUniformMatrix4fv("modelMat", glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * m_scale));
    glDrawArrays(GL_TRIANGLES, 0, m_model->drawArrayCount());
  }

  if (m_isTraining) {
    m_isTraining = m_lattice->input<3>(m_model->positions()[m_random->get()]);
  }

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(m_window);
  ImGui::NewFrame();
  ImGui::Begin("Surface Fitting");

  ImVec2 const btnSize(120, 30);
  ImGuiStyle& style = ImGui::GetStyle();
  style.FrameRounding = 3.0f;
  style.FrameBorderSize = 1.0f;

  if (ImGui::TreeNodeEx("SOM Control", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Text("Iterations: %d", m_lattice->currentIteration());
    if (ImGui::Button("Start", btnSize)) {
      m_isTraining = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop", btnSize)) {
      m_isTraining = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset", btnSize)) {
      m_isTraining = false;
      delete m_lattice;
      m_lattice = new Lattice(s_dimen, s_iterNum, s_rate);
      m_latticeIndices = m_lattice->indices();
      s_iterNum = m_lattice->maxIterations();
      s_dimen = m_lattice->dimension();
      s_rate = m_lattice->initialRate();
    }
    ImGui::TreePop();
  }

  if (ImGui::TreeNodeEx("SOM Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::SetNextItemWidth(200);
    if (ImGui::InputInt("Max Iterations##iter", &s_iterNum, 500, 2000)) {
      if (s_iterNum < 0) {
        s_iterNum = 0;
      }
    }
    ImGui::SetNextItemWidth(200);
    if (ImGui::InputInt("Lattice Dimensions##dimen", &s_dimen, 1, 5)) {
      if (s_dimen < 2) {
        s_dimen = 2;
      } else if (s_dimen > 128) {
        s_dimen = 128;
      }
    }
    ImGui::SetNextItemWidth(200);
    if (ImGui::InputFloat("Learning Rate##rate", &s_rate, 0.05f, 0.1f)) {
      if (s_rate < 0.0f) {
        s_rate = 0.0f;
      } else if (s_rate > 1.0f) {
        s_rate = 1.0f;
      }
    }

    if (ImGui::Button("Confirm and Reset##lattice")) {
      m_isTraining = false;
      delete m_lattice;
      m_lattice = new Lattice(s_dimen, s_iterNum, s_rate);
      m_latticeIndices = m_lattice->indices();
      s_iterNum = m_lattice->maxIterations();
      s_dimen = m_lattice->dimension();
      s_rate = m_lattice->initialRate();

      glDeleteBuffers(1, &m_vboLines);
      glDeleteBuffers(1, &m_ibo);
      glCreateBuffers(1, &m_vboLines);
      glCreateBuffers(1, &m_ibo);
      glBindVertexArray(m_vaoLines);
      glVertexArrayVertexBuffer(m_vaoLines, 0, m_vboLines, 0, 3 * sizeof(float));
      glNamedBufferStorage(
        m_vboLines, m_lattice->neurons().size() * 3 * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
      glNamedBufferStorage(
        m_ibo, m_latticeIndices.size() * sizeof(unsigned short), m_latticeIndices.data(), GL_DYNAMIC_STORAGE_BIT);
    }
    ImGui::TreePop();
  }

  if (ImGui::TreeNodeEx("Render Options", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::Button("Toggle Model", btnSize)) {
      m_showModel = !m_showModel;
    }
    ImGui::SameLine();
    if (ImGui::Button("Toggle Points", btnSize)) {
      m_showPoints = !m_showPoints;
    }
    ImGui::SameLine();
    if (ImGui::Button("Toggle Lines", btnSize)) {
      m_showLines = !m_showLines;
    }
    ImGui::TreePop();
  }

  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
  m_random = new RandomIntNumber<unsigned int>(0, m_model->vertexCount() - 1);
  auto const buffer = m_model->vertexBuffer();
  auto const stride = 3 * sizeof(float);
  glCreateBuffers(1, &m_vbo);
  glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, stride);
  glVertexArrayVertexBuffer(m_vao, 1, m_vbo, m_model->drawArrayCount() * stride, stride);
  glNamedBufferStorage(m_vbo, buffer.size() * sizeof(float), buffer.data(), GL_DYNAMIC_STORAGE_BIT);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  m_shader = new Shader();
  m_shader->Attach(GL_VERTEX_SHADER, "shader/default.vert");
  m_shader->Attach(GL_FRAGMENT_SHADER, "shader/default.frag");
  m_shader->Link();
  /** Grid Nodes END **********************************************************/

  /** Grid Lines **********************************************************/
  glCreateVertexArrays(1, &m_vaoLines);
  glCreateBuffers(1, &m_vboLines);
  glBindVertexArray(m_vaoLines);
  glVertexArrayAttribFormat(m_vaoLines, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(m_vaoLines, 0, m_vboLines, 0, 3 * sizeof(float));
  glNamedBufferStorage(m_vboLines, m_lattice->neurons().size() * 3 * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT);
  glCreateBuffers(1, &m_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
  m_latticeIndices = m_lattice->indices();
  glNamedBufferStorage(
    m_ibo, m_latticeIndices.size() * sizeof(unsigned short), m_latticeIndices.data(), GL_DYNAMIC_STORAGE_BIT);
  glEnableVertexAttribArray(0); // IMPORTANT!

  m_shaderLines = new Shader();
  m_shaderLines->Attach(GL_VERTEX_SHADER, "shader/lines.vert");
  m_shaderLines->Attach(GL_FRAGMENT_SHADER, "shader/lines.frag");
  m_shaderLines->Link();
  /** Grid Lines END ******************************************************/

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplOpenGL3_Init();
  ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);

  importFonts("res/fonts");
}

void
MainWindow::onProcessEvent(SDL_Event event)
{
  ImGui_ImplSDL2_ProcessEvent(&event);

  auto const& io = ImGui::GetIO();
  if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
    m_skipEventHandling = true;
    return;
  }

  m_skipEventHandling = false;
}

void
MainWindow::onWindowResized()
{
  m_camera->SetAspectRatio(m_width, m_height);
}

void
MainWindow::onKeyDown(SDL_KeyboardEvent keyEvent)
{
  switch (keyEvent.keysym.sym) {
  case SDLK_q:
    if (keyEvent.keysym.mod & KMOD_CTRL) {
      m_alive = false;
    }
    break;
  case SDLK_r:
    delete m_camera;
    m_camera = new Camera(m_width, m_height);
    break;
  }
}

void
MainWindow::onMouseButtonDown(SDL_MouseButtonEvent buttonEvent)
{
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

void
MainWindow::importFonts(std::filesystem::path dir_path)
{
  namespace fs = std::filesystem;

  if (!fs::exists(dir_path) || !fs::is_directory(dir_path))
    return;

  ImGuiIO& io = ImGui::GetIO();
  for (fs::directory_entry entry : fs::recursive_directory_iterator(dir_path)) {
    if (!entry.is_regular_file())
      continue;
    auto const ext = entry.path().extension().string();
    if (ext != ".ttf" && ext != ".otf")
      continue;
    io.Fonts->AddFontFromFileTTF(entry.path().c_str(), 18);
  }
}
