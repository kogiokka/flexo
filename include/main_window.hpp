#pragma once

#include "opengl_window.hpp"

#include <wx/button.h>
#include <wx/dcclient.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/glcanvas.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>

class MainWindow : public wxFrame
{
  OpenGLWindow* canvas_;

public:
  MainWindow(wxWindow* parent = 0);
  virtual ~MainWindow();
  void InitializeGL();
  void ResetCamera(wxCommandEvent& evt);
  void OnExit(wxCommandEvent& evt);

  wxDECLARE_EVENT_TABLE();
};
