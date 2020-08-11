#pragma once

#include "camera.hpp"
#include "lattice.hpp"
#include "model.hpp"
#include "sdl_opengl_window.hpp"
#include "shader.hpp"

#include <glm/glm.hpp>

#include <iostream>
#include <string>

class MainWindow : public SDLOpenGLWindow
{
  GLuint m_vao;
  GLuint m_vbo;
  GLuint m_vboIdx;
  Shader* m_shader;
  Camera* m_camera;
  Lattice* m_lattice;
  Model* m_model;

public:
  MainWindow(std::string name, int width, int height);
  ~MainWindow();
  virtual void initializeGL() override;
  virtual void paintGL() override;
  virtual void onMouseButtonDown(SDL_MouseButtonEvent button) override;
  virtual void onMouseMotion(SDL_MouseMotionEvent motion) override;
  virtual void onMouseWheel(SDL_MouseWheelEvent wheel) override;
  virtual void onKeyDown(SDL_KeyboardEvent key) override;
  virtual void onWindowResized() override;
};
