#pragma once

#include "glad/glad.h"

#include "Camera.hpp"
#include "Lattice.hpp"
#include "ObjModel.hpp"
#include "RandomIntNumber.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"

#include <wx/dcclient.h>
#include <wx/gdicmn.h>
#include <wx/glcanvas.h>

#include <cassert>
#include <cstddef>
#include <string>

class OpenGLCanvas : public wxGLCanvas
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
  ObjModel* surface_;
  ObjModel* posModel_;
  std::vector<bool> renderOpt_;
  std::vector<unsigned short> linesIdx_;
  std::vector<unsigned short> surfsIdx_;

public:
  enum RenderOpt { MODEL = 0, POINTS, LINES, SURFACE };

public:
  OpenGLCanvas(wxWindow* parent,
               wxGLAttributes const& dispAttrs,
               wxWindowID id = wxID_ANY,
               wxPoint const& pos = wxDefaultPosition,
               wxSize const& size = wxDefaultSize,
               long style = 0,
               wxString const& name = wxEmptyString);
  ~OpenGLCanvas();
  void OnPaint(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnMouseMotion(wxMouseEvent& event);
  void OnMouseWheel(wxMouseEvent& event);
  void OnMouseLeftDown(wxMouseEvent& event);
  void OnMouseRightDown(wxMouseEvent& event);
  void InitGL();
  void ResetCamera();
  void TogglePlayPause(bool toTrain);
  bool GetPlayPause() const;
  void ToggleRenderOption(RenderOpt opt);
  bool GetRenderOptionState(RenderOpt opt) const;

  wxDECLARE_EVENT_TABLE();
};
