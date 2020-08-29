#pragma once

#include "OpenGLCanvas.hpp"

#include <wx/button.h>
#include <wx/dcclient.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/timer.h>

enum {
  BTN_STARTPAUSE = wxID_HIGHEST + 1,
  CB_RENDEROPT_SURFACE,
  CB_RENDEROPT_LAT_VERTEX,
  CB_RENDEROPT_LAT_EDGE,
  CB_RENDEROPT_LAT_FACE,
  TIMER_UI_UPDATE,
};

class MainWindow : public wxFrame
{
  wxTimer* updateTimer_;
  wxPanel* panel_;
  wxButton* btnStartPause_;
  wxTextCtrl* currIter_;
  OpenGLCanvas* canvas_;

  void CreateOpenGLCanvas();

public:
  MainWindow(wxWindow* parent = 0);
  virtual ~MainWindow();
  void InitializeGL();
  void ResetCamera(wxCommandEvent& evt);
  void OnUpdateTimer(wxTimerEvent& evt);
  void OnButtonStartPause(wxCommandEvent& evt);
  void OnCheckBoxToggleModel(wxCommandEvent& evt);
  void OnCheckBoxTogglePoints(wxCommandEvent& evt);
  void OnCheckBoxToggleLines(wxCommandEvent& evt);
  void OnCheckBoxToggleSurfaces(wxCommandEvent& evt);
  void OnExit(wxCommandEvent& evt);

  wxDECLARE_EVENT_TABLE();
};
