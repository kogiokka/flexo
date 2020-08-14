#pragma once

#include <SDL.h>
#include <glad/glad.h>

#include <string>

class SDLOpenGLWindow
{
protected:
  bool m_alive;
  bool m_skipEventHandling;
  int m_width;
  int m_height;
  SDL_Window* m_window;
  SDL_GLContext m_glContext;
  void handleEvent(SDL_Event event);
  virtual void onKeyDown(SDL_KeyboardEvent keyEvent);
  virtual void onKeyUp(SDL_KeyboardEvent keyEvent);
  virtual void onMouseButtonDown(SDL_MouseButtonEvent buttonEvent);
  virtual void onMouseButtonUp(SDL_MouseButtonEvent buttonEvent);
  virtual void onMouseMotion(SDL_MouseMotionEvent motionEvent);
  virtual void onMouseWheel(SDL_MouseWheelEvent wheelEvent);
  virtual void onWindowResized();
  virtual void onProcessEvent(SDL_Event event);

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
