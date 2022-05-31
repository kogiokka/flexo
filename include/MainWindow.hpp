#pragma once

#include "OpenGLCanvas.hpp"
#include "Lattice.hpp"

#include <wx/button.h>
#include <wx/dcclient.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/timer.h>

class MainWindow : public wxFrame
{
    int widthLat_;
    int heightLat_;
    int iterationCap_;
    float initLearningRate_;
    LatticeFlags latFlags_;

    wxButton* btnPlayPause_;
    wxButton* btnConfirm_;
    wxTimer* timerUIUpdate_;
    wxPanel* panel_;
    wxTextCtrl* tcIterCurr_;
    wxTextCtrl* tcLatWidth_;
    wxTextCtrl* tcLatHeight_;
    wxTextCtrl* tcIterCap_;
    wxTextCtrl* tcInitLearningRate_;
    wxSlider* slider_;
    OpenGLCanvas* canvas_;

    void CreateOpenGLCanvas();
    wxStaticBoxSizer* CreatePanelStaticBox1();
    wxStaticBoxSizer* CreatePanelStaticBox2();
    wxStaticBoxSizer* CreatePanelStaticBox3();

    void OnTimerUIUpdate(wxTimerEvent& evt);
    void OnButtonPlayPause(wxCommandEvent& evt);
    void OnButtonConfirmAndReset(wxCommandEvent& evt);
    void OnCheckboxInputDataset(wxCommandEvent& evt);
    void OnCheckboxLatticeVertex(wxCommandEvent& evt);
    void OnCheckboxLatticeEdge(wxCommandEvent& evt);
    void OnCheckboxLatticeFace(wxCommandEvent& evt);
    void OnCheckboxLightSource(wxCommandEvent& evt);
    void OnCheckboxLatticeFlagsCyclicX(wxCommandEvent& evt);
    void OnCheckboxLatticeFlagsCyclicY(wxCommandEvent& evt);
    void OnSliderTransparency(wxCommandEvent& evt);
    void OnOpenFile(wxCommandEvent& evt);
    void OnExit(wxCommandEvent& evt);

public:
    MainWindow(wxWindow* parent = 0);
    virtual ~MainWindow();
    void InitializeGL();
    void OnMenuCameraReset(wxCommandEvent& evt);

    wxDECLARE_EVENT_TABLE();
};
