#pragma once

#include "OpenGLCanvas.hpp"

#include <wx/button.h>
#include <wx/dcclient.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/textctrl.h>
#include <wx/timer.h>

enum {
  BTN_STARTPAUSE = wxID_HIGHEST + 1,
  BTN_CONFIRM_AND_RESET,
  CB_RENDEROPT_SURFACE,
  CB_RENDEROPT_LAT_VERTEX,
  CB_RENDEROPT_LAT_EDGE,
  CB_RENDEROPT_LAT_FACE,
  TC_SET_ITERATION_CAP,
  TC_SET_LEARNING_RATE,
  TC_SET_DIMENSION,
  SLIDER_TRANSPARENCY,
  TIMER_UI_UPDATE,
};

class MainWindow : public wxFrame
{
  int iterationCap_;
  float initLearningRate_;
  int dimension_;

  wxTimer* updateTimer_;
  wxPanel* panel_;
  wxButton* btnStartPause_;
  wxTextCtrl* tcCurrIter_;
  wxTextCtrl* tcIterCap_;
  wxTextCtrl* tcLearnRate_;
  wxTextCtrl* tcDimen_;
  wxSlider* slider_;
  OpenGLCanvas* canvas_;

  void CreateOpenGLCanvas();
  wxStaticBoxSizer* const CreatePanelStaticBox1();
  wxStaticBoxSizer* const CreatePanelStaticBox2();
  wxStaticBoxSizer* const CreatePanelStaticBox3();

public:
  MainWindow(wxWindow* parent = 0);
  virtual ~MainWindow();
  void InitializeGL();
  void ResetCamera(wxCommandEvent& evt);
  void OnUpdateTimer(wxTimerEvent& evt);
  void OnButtonStartPause(wxCommandEvent& evt);
  void OnButtonConfirmAndReset(wxCommandEvent& evt);
  void OnEnterIterationCap(wxCommandEvent& evt);
  void OnEnterLearningRate(wxCommandEvent& evt);
  void OnEnterDimension(wxCommandEvent& evt);
  void OnCheckBoxToggleModel(wxCommandEvent& evt);
  void OnCheckBoxTogglePoints(wxCommandEvent& evt);
  void OnCheckBoxToggleLines(wxCommandEvent& evt);
  void OnCheckBoxToggleSurfaces(wxCommandEvent& evt);
  void OnSliderTransparency(wxCommandEvent& evt);
  void OnExit(wxCommandEvent& evt);

  wxDECLARE_EVENT_TABLE();
};
