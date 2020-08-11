#pragma once

#include <SDL.h>
#include <glad/glad.h>

#include <string>

class SDLOpenGLWindow
{
protected:
  bool m_alive;
  int m_width;
  int m_height;
  SDL_Window* m_window;
  SDL_GLContext m_glContext;
  void handleEvent(SDL_Event event);
  virtual void resizeGL();
  virtual void onKeyDown(SDL_KeyboardEvent key);
  virtual void onKeyUp(SDL_KeyboardEvent key);
  virtual void onMouseButtonDown(SDL_MouseButtonEvent button);
  virtual void onMouseButtonUp(SDL_MouseButtonEvent button);
  virtual void onMouseMotion(SDL_MouseMotionEvent motion);
  virtual void onMouseWheel(SDL_MouseWheelEvent wheel);

public:
  SDLOpenGLWindow(std::string name, int width, int height);
  ~SDLOpenGLWindow();
  SDL_Window* window();
  SDL_GLContext glContext();
  int width() const;
  int height() const;
  virtual void initializeGL() = 0;
  virtual void paintGL() = 0;

  void show();
};
