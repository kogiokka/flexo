#include "sdl_opengl_window.hpp"

SDLOpenGLWindow::SDLOpenGLWindow(std::string name, int width, int height)
  : m_alive(true)
  , m_width(width)
  , m_height(height)
  , m_window(nullptr)
  , m_glContext(nullptr)
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

  auto const flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  m_window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
  SDL_SetWindowMinimumSize(m_window, m_width, m_height);
  m_glContext = SDL_GL_CreateContext(m_window);
  SDL_GL_MakeCurrent(m_window, m_glContext);
  SDL_GL_SetSwapInterval(1);

  gladLoadGLLoader(SDL_GL_GetProcAddress);
}

void
SDLOpenGLWindow::show()
{
  initializeGL(); // Require implementation

  SDL_Event event;
  while (m_alive) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
        case SDL_WINDOWEVENT_RESIZED:
          resizeGL(); // Require implementation
          break;
        }
        break;
      case SDL_QUIT:
        m_alive = false;
        break;
      }

      handleEvent(event); // Require implementation
    }
    paintGL(); // Require implementation
  }
}

SDLOpenGLWindow::~SDLOpenGLWindow()
{
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

SDL_Window*
SDLOpenGLWindow::window()
{
  return m_window;
}

SDL_GLContext
SDLOpenGLWindow::glContext()
{
  return m_glContext;
}

int
SDLOpenGLWindow::width() const
{
  return m_width;
}

int
SDLOpenGLWindow::height() const
{
  return m_height;
}
