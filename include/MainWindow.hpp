#pragma once

#include "OpenGLCanvas.hpp"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dcclient.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/glcanvas.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statbox.h>

enum {
  BTN_STARTPAUSE = wxID_HIGHEST + 1,
  CB_TOGGLE_MODEL,
  CB_TOGGLE_POINTS,
  CB_TOGGLE_LINES,
  CB_TOGGLE_SURFACES,
};

class MainWindow : public wxFrame
{
  wxPanel* panel_;
  wxButton* btnStartPause_;
  OpenGLCanvas* canvas_;

  void CreateOpenGLCanvas();

public:
  MainWindow(wxWindow* parent = 0);
  virtual ~MainWindow();
  void InitializeGL();
  void ResetCamera(wxCommandEvent& evt);
  void OnButtonStartPause(wxCommandEvent& evt);
  void OnCheckBoxToggleModel(wxCommandEvent& evt);
  void OnCheckBoxTogglePoints(wxCommandEvent& evt);
  void OnCheckBoxToggleLines(wxCommandEvent& evt);
  void OnCheckBoxToggleSurfaces(wxCommandEvent& evt);
  void OnExit(wxCommandEvent& evt);

  wxDECLARE_EVENT_TABLE();
};
