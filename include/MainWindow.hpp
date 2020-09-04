#pragma once

#include "OpenGLCanvas.hpp"

#include <wx/dcclient.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/spinctrl.h>

enum {
  BTN_START = wxID_HIGHEST + 1,
  BTN_PAUSE,
  BTN_CONFIRM_AND_RESET,
  SPCTRL_ITERATION_PER_FRAME,
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

  wxTimer* timerUIUpdate_;
  wxPanel* panel_;
  wxTextCtrl* txtCtrlIterCurr_;
  wxTextCtrl* txtCtrlIterCap_;
  wxTextCtrl* txtCtrlLearningRate_;
  wxTextCtrl* txtCtrlDimension_;
  wxSlider* slider_;
  OpenGLCanvas* canvas_;

  void CreateOpenGLCanvas();
  wxStaticBoxSizer* const CreatePanelStaticBox1();
  wxStaticBoxSizer* const CreatePanelStaticBox2();
  wxStaticBoxSizer* const CreatePanelStaticBox3();

  void OnTimerUIUpdate(wxTimerEvent& evt);
  void OnButtonStart(wxCommandEvent& evt);
  void OnButtonPause(wxCommandEvent& evt);
  void OnButtonConfirmAndReset(wxCommandEvent& evt);
  void OnSpinCtrlIterationPerFrame(wxSpinEvent& evt);
  void OnTextCtrlIterationCap(wxCommandEvent& evt);
  void OnTextCtrlLearningRate(wxCommandEvent& evt);
  void OnTextCtrlDimension(wxCommandEvent& evt);
  void OnCheckboxSurface(wxCommandEvent& evt);
  void OnCheckboxLatticeVertex(wxCommandEvent& evt);
  void OnCheckboxLatticeEdge(wxCommandEvent& evt);
  void OnCheckboxLatticeFace(wxCommandEvent& evt);
  void OnSliderTransparency(wxCommandEvent& evt);
  void OnOpen(wxCommandEvent& evt);
  void OnExit(wxCommandEvent& evt);

public:
  MainWindow(wxWindow* parent = 0);
  virtual ~MainWindow();
  void InitializeGL();
  void OnMenuCameraReset(wxCommandEvent& evt);

  wxDECLARE_EVENT_TABLE();
};
