#pragma once

#include "opengl_window.hpp"

#include <wx/dcclient.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/glcanvas.h>

class MainWindow : public wxFrame
{
  OpenGLWindow* canvas_;

public:
  MainWindow(wxWindow* parent = 0);
  virtual ~MainWindow();
  void InitializeGL();
  void ResetCamera(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);

  wxDECLARE_EVENT_TABLE();
};
