#pragma once

#include "camera.hpp"
#include "glad/glad.h"
#include "lattice.hpp"
#include "obj_model.hpp"
#include "random_int_number.hpp"
#include "shader.hpp"
#include "vertex_array.hpp"

#include <wx/gdicmn.h>
#include <wx/glcanvas.h>

#include <cassert>
#include <cstddef>
#include <string>

class OpenGLWindow : public wxGLCanvas
{
  bool isGLLoaded_;
  bool toTrain_;
  GLuint vboSurf_;
  GLuint vboPosModel_;
  GLuint vboLatPos_;
  GLuint iboLatLines_;
  GLuint iboLatSurf_;
  RandomIntNumber<unsigned int>* random_;
  VertexArray* vao_;
  wxGLContext* context_;
  Shader* shader_;
  Shader* shaderLines_;
  Shader* shaderNodes_;
  Camera* camera_;
  Lattice* lattice_;
  OBJModel* surface_;
  OBJModel* posModel_;
  std::vector<unsigned short> linesIdx_;
  std::vector<unsigned short> surfsIdx_;

public:
  OpenGLWindow(wxWindow* parent,
               wxGLAttributes const& dispAttrs,
               wxWindowID id = wxID_ANY,
               wxPoint const& pos = wxDefaultPosition,
               wxSize const& size = wxDefaultSize,
               long style = 0,
               wxString const& name = wxEmptyString);
  ~OpenGLWindow();
  void OnPaint(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnMouseMotion(wxMouseEvent& event);
  void OnMouseWheel(wxMouseEvent& event);
  void OnMouseLeftDown(wxMouseEvent& event);
  void OnMouseRightDown(wxMouseEvent& event);
  void InitGL();
  void ResetCamera();
  void ToggleTrainPause(bool toTrain);
  bool GetTrainPause() const;

  wxDECLARE_EVENT_TABLE();
};
