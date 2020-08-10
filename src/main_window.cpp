#include "main_window.hpp"

MainWindow::MainWindow(std::string name, int width, int height)
  : SDLOpenGLWindow(name, width, height)
  , m_vao(0)
  , m_vbo(0)
  , m_shader(nullptr)
  , m_lattice(nullptr)
{
  initialize();
}

MainWindow::~MainWindow()
{
  delete m_lattice;
  delete m_shader;
}

void
MainWindow::paintGL()
{
  bool quit = false;
  SDL_Event evt;

  while (!quit) {
    while (SDL_PollEvent(&evt)) {
      switch (evt.type) {
      case SDL_KEYDOWN:
        if (evt.key.keysym.sym == SDLK_q) {
          if (evt.key.keysym.mod & KMOD_CTRL) {
            quit = true;
          }
        }
        break;
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_WINDOWEVENT:
        switch (evt.window.event) {
        case SDL_WINDOWEVENT_RESIZED:
          resizeGL();
          break;
        }
      }
    }
    glViewport(0, 0, m_width, m_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shader->SetUniformMatrix4fv("view_proj_matrix", viewProjMatrix(m_width, m_height, 1.0f));
    auto const vert_buffer = m_lattice->vertexBuffer();
    glNamedBufferSubData(m_vbo, 0, vert_buffer.size() * sizeof(float), vert_buffer.data());
    glDrawArrays(GL_TRIANGLES, 0, vert_buffer.size());
    SDL_GL_SwapWindow(m_window);
  }
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
      std::cerr << "Type: " << std::hex << type;
      std::cerr << "Severity: " << std::hex << severity;
      std::cerr << "Message: " << message << "\n";
    },
    nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

  m_lattice = new Lattice(32, 1000, 0.1f);

  std::vector<float> const vert_buffer = m_lattice->vertexBuffer();
  int const stride = 5 * sizeof(float);

  glCreateVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  glVertexArrayAttribFormat(m_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(m_vao, 1, 3, GL_FLOAT, GL_FALSE, 0);

  glCreateBuffers(1, &m_vbo);
  glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, stride);
  glVertexArrayVertexBuffer(m_vao, 1, m_vbo, 2 * sizeof(float), stride);
  glNamedBufferStorage(m_vbo, vert_buffer.size() * sizeof(float), vert_buffer.data(), GL_DYNAMIC_STORAGE_BIT);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  m_shader = new Shader();
  m_shader->Attach(GL_VERTEX_SHADER, "shader/default.vert");
  m_shader->Attach(GL_FRAGMENT_SHADER, "shader/default.frag");
  m_shader->Link();
  m_shader->Use();
}

glm::mat4
MainWindow::viewProjMatrix(int width, int height, float zoom)
{
  float const aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
  int const dimen = m_lattice->dimension();

  float const h = 0.6f * dimen * zoom;
  float const v = 0.6f * dimen * aspect_ratio * zoom;

  float const cx = dimen * 0.5f;
  float const cy = dimen * 0.5f;
  return glm::ortho(-v, v, -h, h, 0.0f, 100.0f) *
         glm::lookAt(glm::vec3{cx, cy, 1.0f}, glm::vec3{cx, cy, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
}

void
MainWindow::resizeGL()
{
  SDL_GetWindowSize(m_window, &m_width, &m_height);
}
