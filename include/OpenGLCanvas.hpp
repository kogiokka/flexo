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
  GLuint vboLatFace_;
  GLuint vboLatPos_;
  GLuint iboLatEdge_;
  GLuint vboVertModel_;
  float surfaceTransparency_;
  int iterPerFrame_;
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
  std::vector<unsigned int> latEdgeIndices_;
  std::vector<unsigned int> latFaceIndices_;

public:
  enum RenderOpt { SURFACE = 0, LAT_VERTEX, LAT_EDGE, LAT_FACE };

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
  void ResetLattice(int iterationCap, float initLearningRate, int dimension);
  void ToggleRenderOption(RenderOpt opt);
  void SetPlayOrPause(bool toTrain);
  void SetSurfaceTransparency(float alpha);
  void SetIterationsPerFrame(int times);
  void OpenSurface(std::string const& path);
  int GetIterationCap() const;
  int GetLatticeDimension() const;
  int GetCurrentIterations() const;
  int GetIterationsPerFrame() const;
  bool GetRenderOptionState(RenderOpt opt) const;
  float GetInitialLearningRate() const;
  float GetSurfaceTransparency() const;

  wxDECLARE_EVENT_TABLE();
};
