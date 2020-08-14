#pragma once

#include "camera.hpp"
#include "lattice.hpp"
#include "model.hpp"
#include "random_int_number.hpp"
#include "sdl_opengl_window.hpp"
#include "shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

#include <filesystem>
#include <iostream>
#include <string>

class MainWindow : public SDLOpenGLWindow
{
  GLuint m_vao;
  GLuint m_vbo;
  GLuint m_vaoLines;
  GLuint m_vboPos;
  GLuint m_iboLines;
  float m_scale;
  bool m_showModel;
  bool m_showPoints;
  bool m_showLines;
  bool m_isTraining;
  Shader* m_shader;
  Shader* m_shaderLines;
  Shader* m_shaderNodes;
  Camera* m_camera;
  Lattice* m_lattice;
  Model* m_model;
  RandomIntNumber<unsigned int>* m_random;
  std::vector<unsigned short> m_latticeIndices;

  void importFonts(std::filesystem::path dir_path);

public:
  MainWindow(std::string name, int width, int height);
  ~MainWindow();
  virtual void initializeGL() override;
  virtual void paintGL() override;
  virtual void onMouseButtonDown(SDL_MouseButtonEvent buttonEvent) override;
  virtual void onMouseMotion(SDL_MouseMotionEvent motionEvent) override;
  virtual void onMouseWheel(SDL_MouseWheelEvent wheelEvent) override;
  virtual void onKeyDown(SDL_KeyboardEvent keyEvent) override;
  virtual void onWindowResized() override;
  virtual void onProcessEvent(SDL_Event event) override;
};
