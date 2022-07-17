#ifndef MAIN_PANEL_H
#define MAIN_PANEL_H

#include <memory>

#include <wx/button.h>
#include <wx/event.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/window.h>

#include "OpenGLCanvas.hpp"
#include "Project.hpp"

enum {
    TE_MAX_ITERATIONS = wxID_HIGHEST + 1,
    TE_LEARNING_RATE,
    TE_NEIGHBORHOOD,
    TE_LATTICE_WIDTH,
    TE_LATTICE_HEIGHT,
    BTN_PLAY_PAUSE,
    BTN_WATERMARK,
    BTN_CONFIRM_LATTICE,
    BTN_CONFIRM_SOM,
    BTN_START,
    BTN_STOP,
    SPCTRL_ITERATION_PER_FRAME,
    CB_RENDEROPT_MODEL,
    CB_RENDEROPT_LAT_VERTEX,
    CB_RENDEROPT_LAT_EDGE,
    CB_RENDEROPT_LAT_FACE,
    CB_RENDEROPT_LIGHT_SOURCE,
    CB_LATTICE_FLAGS_CYCLIC_X,
    CB_LATTICE_FLAGS_CYCLIC_Y,
    SLIDER_TRANSPARENCY,
    TIMER_UI_UPDATE,
};

class MainPanel : public wxPanel
{
    std::shared_ptr<Lattice> m_lattice;

    WatermarkingProject& m_project;
    wxNotebook* m_notebook;
    wxButton* m_btnStart;
    wxButton* m_btnStop;
    wxButton* m_btnPlayPause;
    wxButton* m_btnWatermark;
    wxButton* m_btnConfirmLattice;
    wxButton* m_btnConfirmSOM;
    wxTextCtrl* m_tcIterations;
    wxTextCtrl* m_tcNeighborhood;
    wxTextCtrl* m_tcLatticeWidth;
    wxTextCtrl* m_tcLatticeHeight;
    wxTextCtrl* m_tcMaxIterations;
    wxTextCtrl* m_tcInitialRate;
    wxTextCtrl* m_tcInitialNeighborhood;
    wxSlider* m_slider;
    wxTimer* m_updateTimer;

public:
    MainPanel(wxWindow* parent, WatermarkingProject& project);
    ~MainPanel();

private:
    void PopulateProjectPage();
    void PopulateRenderingPage();
    void OnButtonStart(wxCommandEvent& evt);
    void OnButtonStop(wxCommandEvent& evt);
    void OnButtonPause(wxCommandEvent& evt);
    void OnButtonWatermark(wxCommandEvent& evt);
    void OnButtonConfirmLattice(wxCommandEvent& evt);
    void OnButtonConfirmSOM(wxCommandEvent& evt);
    void OnCheckboxModel(wxCommandEvent& evt);
    void OnCheckboxLatticeVertex(wxCommandEvent& evt);
    void OnCheckboxLatticeEdge(wxCommandEvent& evt);
    void OnCheckboxLatticeFace(wxCommandEvent& evt);
    void OnCheckboxLightSource(wxCommandEvent& evt);
    void OnCheckboxLatticeFlagsCyclicX(wxCommandEvent& evt);
    void OnCheckboxLatticeFlagsCyclicY(wxCommandEvent& evt);
    void OnSliderTransparency(wxCommandEvent& evt);
    void OnTimerUIUpdate(wxTimerEvent& evt);
    void OnUpdateStartButton(wxUpdateUIEvent& evt);
    void OnUpdateStopButton(wxUpdateUIEvent& evt);
    void OnSetLatticeWidth(wxCommandEvent& evt);
    void OnSetLatticeHeight(wxCommandEvent& evt);
    void OnSetMaxIterations(wxCommandEvent& evt);
    void OnSetLearningRate(wxCommandEvent& evt);
    void OnSetNeighborhood(wxCommandEvent& evt);

    wxDECLARE_EVENT_TABLE();
};

wxDECLARE_EVENT(CMD_TOGGLE_RENDER_FLAG, wxCommandEvent);
wxDECLARE_EVENT(CMD_TOGGLE_LATTICE_FLAG, wxCommandEvent);
wxDECLARE_EVENT(CMD_DO_WATERMARK, wxCommandEvent);
wxDECLARE_EVENT(CMD_CREATE_LATTICE, wxCommandEvent);
wxDECLARE_EVENT(CMD_CREATE_SOM_PROCEDURE, wxCommandEvent);

#endif
