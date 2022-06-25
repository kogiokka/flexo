#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

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

#include "Lattice.hpp"
#include "OpenGLCanvas.hpp"

class MainWindow : public wxFrame
{
    int m_widthLat;
    int m_heightLat;
    int m_iterationCap;
    float m_initLearningRate;
    LatticeFlags m_latFlags;

    wxButton* m_btnPlayPause;
    wxButton* m_btnWatermark;
    wxButton* m_btnConfirm;
    wxTimer* m_timerUIUpdate;
    wxPanel* m_panel;
    wxTextCtrl* m_tcIterCurr;
    wxTextCtrl* m_tcLatWidth;
    wxTextCtrl* m_tcLatHeight;
    wxTextCtrl* m_tcIterCap;
    wxTextCtrl* m_tcInitLearningRate;
    wxSlider* m_slider;
    OpenGLCanvas* m_canvas;

    void CreateOpenGLCanvas();
    wxStaticBoxSizer* CreatePanelStaticBox1();
    wxStaticBoxSizer* CreatePanelStaticBox2();
    wxStaticBoxSizer* CreatePanelStaticBox3();

    void OnTimerUIUpdate(wxTimerEvent& evt);
    void OnButtonPlayPause(wxCommandEvent& evt);
    void OnButtonWatermark(wxCommandEvent& evt);
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

#endif
