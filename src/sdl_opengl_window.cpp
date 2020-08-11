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

SDLOpenGLWindow::~SDLOpenGLWindow()
{
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

void
SDLOpenGLWindow::show()
{
  initializeGL(); // Require implementation

  while (m_alive) {
    SDL_Event event;
    handleEvent(event);
    paintGL(); // Require implementation
  }
}

void
SDLOpenGLWindow::handleEvent(SDL_Event event)
{
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_KEYDOWN:
      onKeyDown(event.key);
      break;
    case SDL_KEYUP:
      onKeyUp(event.key);
      break;
    case SDL_MOUSEBUTTONDOWN:
      onMouseButtonDown(event.button);
      break;
    case SDL_MOUSEBUTTONUP:
      onMouseButtonUp(event.button);
      break;
    case SDL_MOUSEMOTION:
      onMouseMotion(event.motion);
      break;
    case SDL_MOUSEWHEEL:
      onMouseWheel(event.wheel);
      break;
    case SDL_WINDOWEVENT:
      switch (event.window.event) {
      case SDL_WINDOWEVENT_RESIZED:
        resizeGL();
        break;
      }
      break;
    case SDL_QUIT:
      m_alive = false;
      break;
    }
  }
}

void SDLOpenGLWindow::onKeyDown(SDL_KeyboardEvent)
{
  return; // Override this function to do things.
}

void SDLOpenGLWindow::onKeyUp(SDL_KeyboardEvent)
{
  return; // Override this function to do things.
}

void SDLOpenGLWindow::onMouseButtonDown(SDL_MouseButtonEvent)
{
  return; // Override this function to do things.
}

void SDLOpenGLWindow::onMouseButtonUp(SDL_MouseButtonEvent)
{
  return; // Override this function to do things.
}

void SDLOpenGLWindow::onMouseMotion(SDL_MouseMotionEvent)
{
  return; // Override this function to do things.
}

void SDLOpenGLWindow::onMouseWheel(SDL_MouseWheelEvent)
{
  return; // Override this function to do things.
}

void
SDLOpenGLWindow::resizeGL()
{
  SDL_GetWindowSize(m_window, &m_width, &m_height);
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
