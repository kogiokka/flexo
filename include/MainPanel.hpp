#ifndef MAIN_PANEL_H
#define MAIN_PANEL_H

#include <memory>

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/window.h>

#include "Lattice.hpp"
#include "OpenGLCanvas.hpp"
#include "Renderer.hpp"

class MainWindow;

class MainPanel : public wxPanel
{
    friend MainWindow;
    std::shared_ptr<Lattice> m_lattice;

    OpenGLCanvas* m_canvas;
    wxButton* m_btnPlayPause;
    wxButton* m_btnWatermark;
    wxButton* m_btnConfirm;
    wxTextCtrl* m_tcIterCurr;
    wxTextCtrl* m_tcLatWidth;
    wxTextCtrl* m_tcLatHeight;
    wxTextCtrl* m_tcIterCap;
    wxTextCtrl* m_tcInitLearningRate;
    wxSlider* m_slider;
    wxTimer* m_updateTimer;

public:
    MainPanel(wxWindow* parent = nullptr);
    ~MainPanel();
    void SetupTraining();
    void SetOpenGLCanvas(OpenGLCanvas* canvas);

private:
    wxStaticBoxSizer* CreatePanelStaticBox1();
    wxStaticBoxSizer* CreatePanelStaticBox2();
    wxStaticBoxSizer* CreatePanelStaticBox3();
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
    void OnTimerUIUpdate(wxTimerEvent& evt);
    void ToggleRenderOption(RenderOption opt);

    wxDECLARE_EVENT_TABLE();
};

#endif
