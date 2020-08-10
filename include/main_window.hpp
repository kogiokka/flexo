#pragma once

#include "lattice.hpp"
#include "sdl_opengl_window.hpp"
#include "shader.hpp"

#include <glm/glm.hpp>

#include <string>
#include <iostream>

class MainWindow : public SDLOpenGLWindow
{
  GLuint m_vao;
  GLuint m_vbo;
  Shader* m_shader;
  Lattice* m_lattice;

  glm::mat4 viewProjMatrix(int width, int height, float zoom);

public:
  MainWindow(std::string name, int width, int height);
  ~MainWindow();
  virtual void initializeGL() override;
  virtual void resizeGL() override;
  virtual void paintGL() override;
};
