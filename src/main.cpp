#include "lattice.hpp"
#include "random_vec3.hpp"
#include "shader.hpp"

#include <SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

static int width = 1200;
static int height = 800;

static int const dimen = 128;
static int iterations = 1000;
static float init_learning_rate = 0.1f;
static bool training = false;

glm::mat4
viewProjMatrix(int width, int height, float zoom)
{
  float const aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
  float const h = 0.6f * dimen * zoom;
  float const v = 0.6f * dimen * aspect_ratio * zoom;

  float const cx = dimen * 0.5f;
  float const cy = dimen * 0.5f;
  return glm::ortho(-v, v, -h, h, 0.0f, 100.0f) *
         glm::lookAt(glm::vec3{cx, cy, 1.0f}, glm::vec3{cx, cy, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
}

void
importFonts(std::filesystem::path dir_path)
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

int
main(int argc, char** argv)
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

#ifndef NDEBUG
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_RELEASE_BEHAVIOR, 0);
#endif

  auto const win_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  std::unique_ptr<SDL_Window, decltype((SDL_DestroyWindow))> window{
    SDL_CreateWindow("SOM - Surface Fitting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, win_flags),
    SDL_DestroyWindow};

  SDL_SetWindowMinimumSize(window.get(), width, height);
  SDL_GLContext gl_cxt = SDL_GL_CreateContext(window.get());
  SDL_GL_MakeCurrent(window.get(), gl_cxt);
  gladLoadGLLoader(SDL_GL_GetProcAddress);
  SDL_GL_SetSwapInterval(1);

#ifndef NDEBUG
  glDebugMessageCallback(
    [](GLenum source,
       GLenum type,
       GLuint id,
       GLenum severity,
       GLsizei length,
       GLchar const* message,
       void const* user_param) noexcept {
      fprintf(stderr, "Type: %x, Severity: %x Message: %s.\n", type, severity, message);
    },
    nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

  unsigned int vao;
  unsigned int vbo;
  int const stride = 5 * sizeof(float);
  int const vert_count = (dimen - 1) * (dimen - 1) * 5 * 6;
  RandomVec3 rc;
  Lattice lattice(dimen, iterations, init_learning_rate);

  glCreateVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 0);

  glCreateBuffers(1, &vbo);
  glVertexArrayVertexBuffer(vao, 0, vbo, 0, stride);
  glVertexArrayVertexBuffer(vao, 1, vbo, 2 * sizeof(float), stride);
  glNamedBufferStorage(vbo, vert_count * sizeof(float), lattice.vertexBuffer().data(), GL_DYNAMIC_STORAGE_BIT);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  auto shader = std::make_unique<Shader>();
  shader->Attach(GL_VERTEX_SHADER, "shader/default.vert");
  shader->Attach(GL_FRAGMENT_SHADER, "shader/default.frag");
  shader->Link();
  shader->Use();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplOpenGL3_Init();
  ImGui_ImplSDL2_InitForOpenGL(window.get(), gl_cxt);

  importFonts("res/fonts");

  bool quit = false;
  std::string status = "Training...";
  std::string btn_start_pause = "Train";
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
          SDL_GetWindowSize(window.get(), &width, &height);
          break;
        }
      }
    }
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bool finished = lattice.isFinished();
    if (training) {
      btn_start_pause = "Pause";
      for (int i = 0; i < 10; ++i) {
        auto const color = rc.color();
        training = lattice.input({color[0], color[1], color[2]});
      }
    } else {
      if (finished) {
        btn_start_pause = "Finished";
      } else {
        btn_start_pause = "Train";
      }
      status = "Done.";
    }
    glNamedBufferSubData(vbo, 0, vert_count * sizeof(float), lattice.vertexBuffer().data());
    shader->SetUniformMatrix4fv("view_proj_matrix", viewProjMatrix(width, height, 1.0f));

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window.get());
    ImGui::NewFrame();
    ImGui::Begin("SOM");
    ImGui::Text("Iterations: %d", lattice.iterations());
    ImGui::Text("Learning Rate: %.6f", lattice.learningRate());
    ImGui::Text("Neighborhood: %.6f", lattice.neighborhoodRadius());
    ImGui::Text("Status: %s", status.c_str());
    if (ImGui::Button(btn_start_pause.c_str())) {
      if (!finished) {
        training = !training;
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
      training = false;
      lattice = Lattice(dimen, iterations, init_learning_rate);
    }
    glDrawArrays(GL_TRIANGLES, 0, vert_count);
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window.get());
  }

  SDL_Quit();
  return 0;
}
