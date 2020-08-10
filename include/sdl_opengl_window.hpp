#pragma once

#include <SDL.h>
#include <glad/glad.h>

#include <string>

class SDLOpenGLWindow
{
protected:
  int m_width;
  int m_height;
  SDL_Window* m_window;
  SDL_GLContext m_glContext;

  void initialize();

public:
  SDLOpenGLWindow(std::string name, int width, int height);
  ~SDLOpenGLWindow();
  SDL_Window* window();
  SDL_GLContext glContext();
  int width() const;
  int height() const;
  virtual void initializeGL() = 0;
  virtual void resizeGL() = 0;
  virtual void paintGL() = 0;
};
