#include "main_window.hpp"

MainWindow::MainWindow(std::string name, int width, int height)
  : SDLOpenGLWindow(name, width, height)
  , m_vbo(0)
  , m_vboPos(0)
  , m_scale(50.0f)
  , m_showModel(true)
  , m_showPoints(true)
  , m_showLines(true)
  , m_showSurfs(true)
  , m_isTraining(false)
  , m_vao(nullptr)
  , m_shader(nullptr)
  , m_shaderLines(nullptr)
  , m_shaderNodes(nullptr)
  , m_camera(nullptr)
  , m_lattice(nullptr)
  , m_model(nullptr)
{
  m_lattice = new Lattice(32, 20000, 0.15f);
  m_camera = new Camera(width, height);
  // Some lighting problem with Perspective mode
  // m_camera->SetProjection(Camera::Projection::Perspective);
}

MainWindow::~MainWindow()
{
  delete m_vao;
  delete m_random;
  delete m_model;
  delete m_lattice;
  delete m_camera;
  delete m_shader;
  delete m_shaderLines;
  delete m_shaderNodes;
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
  glNamedBufferSubData(m_vboPos, 0, renderPos.size() * 3 * sizeof(float), renderPos.data());

  if (m_showPoints) {
    m_vao->enable("instanced");
    m_shaderNodes->Use();
    m_shaderNodes->SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
    m_shaderNodes->SetUniform3fv("viewPos", m_camera->Position());
    m_shaderNodes->SetUniform3fv("lightSrc", m_camera->Position());
    m_shaderNodes->SetUniformMatrix4fv("viewProjMat", m_camera->ViewProjectionMatrix());
    m_shaderNodes->SetUniformMatrix4fv("modelMat", scaleMat);
    glVertexArrayVertexBuffer(m_vao->id(), 0, m_vbo, 0, 3 * sizeof(float));
    glDrawArraysInstanced(GL_TRIANGLES, 0, m_model->drawArrayCount(), renderPos.size());
    m_vao->disable("instanced");
  }

  if (m_showSurfs) {
    m_vao->disable("normal");
    m_shaderLines->Use();
    m_shaderLines->SetUniformMatrix4fv("viewProjMat", m_camera->ViewProjectionMatrix());
    m_shaderLines->SetUniformMatrix4fv("modelMat", glm::mat4(1.0f));
    m_shaderLines->SetUniform3f("color", 0.7f, 0.7f, 0.7f);
    glVertexArrayVertexBuffer(m_vao->id(), 0, m_vboPos, 0, 3 * sizeof(float));
    glVertexArrayElementBuffer(m_vao->id(), m_iboSurfs);
    glDrawElements(GL_TRIANGLES, m_surfsIdx.size(), GL_UNSIGNED_SHORT, 0);
    m_vao->enable("normal");
  }

  if (m_showLines) {
    m_vao->disable("normal");
    m_shaderLines->Use();
    m_shaderLines->SetUniformMatrix4fv("viewProjMat", m_camera->ViewProjectionMatrix());
    m_shaderLines->SetUniformMatrix4fv("modelMat", glm::mat4(1.0f));
    m_shaderLines->SetUniform3f("color", 1.0f, 1.0f, 1.0f);
    glVertexArrayVertexBuffer(m_vao->id(), 0, m_vboPos, 0, 3 * sizeof(float));
    glVertexArrayElementBuffer(m_vao->id(), m_iboLines);
    glDrawElements(GL_LINES, m_linesIdx.size(), GL_UNSIGNED_SHORT, 0);
    m_vao->enable("normal");
  }

  if (m_showModel) {
    m_shader->Use();
    m_shader->SetUniformMatrix4fv("viewProjMat", m_camera->ViewProjectionMatrix());
    m_shader->SetUniformMatrix4fv("modelMat", glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * m_scale));
    m_shader->SetUniform3fv("viewPos", m_camera->Position());
    m_shader->SetUniform3fv("lightSrc", m_camera->Position());
    m_shader->SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
    m_shader->SetUniform1f("alpha", 0.6f);
    glVertexArrayVertexBuffer(m_vao->id(), 0, m_vbo, 0, 3 * sizeof(float));
    glDrawArrays(GL_TRIANGLES, 0, m_model->drawArrayCount());
  }

  if (m_isTraining) {
    m_isTraining = m_lattice->input<3>(m_model->positions()[m_random->get()]);
  }

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(m_window);
  ImGui::NewFrame();
  ImVec2 const btnSize(120, 30);

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Camera")) {
      if (ImGui::MenuItem("Reset")) {
        delete m_camera;
        m_camera = new Camera(m_width, m_height);
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  ImGui::SetNextWindowSize(ImVec2(450, 450));
  ImGui::Begin("Surface Fitting", nullptr, ImGuiWindowFlags_NoResize);
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
      } else if (s_dimen > 256) {
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
      bool const isSameIter = (s_iterNum == m_lattice->maxIterations());
      bool const isSameDimen = (s_dimen == m_lattice->dimension());
      bool const isSameRate = (s_rate == m_lattice->initialRate());
      bool const changed = !(isSameIter && isSameDimen && isSameRate);
      delete m_lattice;
      m_lattice = new Lattice(s_dimen, s_iterNum, s_rate);

      if (changed) {
        s_iterNum = m_lattice->maxIterations();
        s_dimen = m_lattice->dimension();
        s_rate = m_lattice->initialRate();
        m_linesIdx = m_lattice->lineIndices();
        m_surfsIdx = m_lattice->triangleIndices();

        glDeleteBuffers(1, &m_iboLines);
        glDeleteBuffers(1, &m_iboSurfs);

        glCreateBuffers(1, &m_iboLines);
        glCreateBuffers(1, &m_iboSurfs);
        glNamedBufferStorage(
          m_iboLines, m_linesIdx.size() * sizeof(unsigned short), m_linesIdx.data(), GL_DYNAMIC_STORAGE_BIT);
        glNamedBufferStorage(
          m_iboSurfs, m_surfsIdx.size() * sizeof(unsigned short), m_surfsIdx.data(), GL_DYNAMIC_STORAGE_BIT);

        glDeleteBuffers(1, &m_vboPos);
        glCreateBuffers(1, &m_vboPos);
        glNamedBufferStorage(
          m_vboPos, m_lattice->neurons().size() * 3 * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayVertexBuffer(m_vao->id(), 2, m_vboPos, 0, 3 * sizeof(float));
      }
    }
    ImGui::TreePop();
  }

  if (ImGui::TreeNodeEx("SOM Control", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Text("Iterations: %d", m_lattice->currentIteration());
    if (ImGui::Button("Start", btnSize)) {
      m_isTraining = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause", btnSize)) {
      m_isTraining = false;
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
    if (ImGui::Button("Toggle Lines", btnSize)) {
      m_showLines = !m_showLines;
    }
    ImGui::SameLine();
    if (ImGui::Button("Toggle Surfaces", btnSize)) {
      m_showSurfs = !m_showSurfs;
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
  m_vao = new VertexArray();
  AttribFormat format;
  format.count = 3;
  format.type = GL_FLOAT;
  format.normalized = GL_FALSE;

  m_vao->addAttrib("position", 0, format);
  m_vao->addAttrib("normal", 1, format);
  m_vao->addAttrib("instanced", 2, format);

  m_model = new Model();
  m_model->readOBJ("res/models/Icosphere.obj");
  m_random = new RandomIntNumber<unsigned int>(0, m_model->vertexCount() - 1);
  auto const buffer = m_model->vertexBuffer();
  auto const stride = 3 * sizeof(float);
  glCreateBuffers(1, &m_vbo);
  glVertexArrayVertexBuffer(m_vao->id(), 0, m_vbo, 0, stride);
  glVertexArrayVertexBuffer(m_vao->id(), 1, m_vbo, m_model->drawArrayCount() * stride, stride);
  glNamedBufferStorage(m_vbo, buffer.size() * sizeof(float), buffer.data(), GL_DYNAMIC_STORAGE_BIT);

  m_shader = new Shader();
  m_shader->Attach(GL_VERTEX_SHADER, "shader/default.vert");
  m_shader->Attach(GL_FRAGMENT_SHADER, "shader/default.frag");
  m_shader->Link();
  /** Grid Nodes END **********************************************************/

  /** Grid Lines **********************************************************/
  m_linesIdx = m_lattice->lineIndices();
  m_surfsIdx = m_lattice->triangleIndices();

  glCreateBuffers(1, &m_vboPos);
  glCreateBuffers(1, &m_iboLines);
  glCreateBuffers(1, &m_iboSurfs);
  glNamedBufferStorage(m_vboPos, m_lattice->neurons().size() * 3 * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferStorage(
    m_iboLines, m_linesIdx.size() * sizeof(unsigned short), m_linesIdx.data(), GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferStorage(
    m_iboSurfs, m_surfsIdx.size() * sizeof(unsigned short), m_surfsIdx.data(), GL_DYNAMIC_STORAGE_BIT);

  m_shaderLines = new Shader();
  m_shaderLines->Attach(GL_VERTEX_SHADER, "shader/lines.vert");
  m_shaderLines->Attach(GL_FRAGMENT_SHADER, "shader/lines.frag");
  m_shaderLines->Link();

  m_shaderNodes = new Shader();
  m_shaderNodes->Attach(GL_VERTEX_SHADER, "shader/nodes.vert");
  m_shaderNodes->Attach(GL_FRAGMENT_SHADER, "shader/nodes.frag");
  m_shaderNodes->Link();
  /** Grid Lines END ******************************************************/

  glVertexArrayVertexBuffer(m_vao->id(), 2, m_vboPos, 0, 3 * sizeof(float));
  glVertexArrayBindingDivisor(m_vao->id(), 2, 1);

  m_vao->enable("position");
  m_vao->enable("normal");
  m_vao->bind();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplOpenGL3_Init();
  ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
  ImGuiStyle& style = ImGui::GetStyle();
  style.FrameRounding = 3.0f;
  style.FrameBorderSize = 1.0f;

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
