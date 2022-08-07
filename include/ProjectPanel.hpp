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
#include <wx/aui/framemanager.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/window.h>

enum {
    TE_MAX_ITERATIONS = wxID_HIGHEST + 1,
    TE_LEARNING_RATE,
    TE_INITIAL_NEIGHBORHOOD,
    TE_LATTICE_WIDTH,
    TE_LATTICE_HEIGHT,
    BTN_INITIALIZE_LATTICE,
    BTN_CREATE_PROJECT,
    BTN_STOP_PROJECT,
    BTN_RUN,
    BTN_WATERMARK,
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
    PANEL_NOTEBOOK,
    PANEL_LATTICE,
    PANEL_SOM,
    PANEL_WATERMARKING
};

class WatermarkingProject;

class ProjectPanel : public wxNotebook
{
    wxAuiManager m_auiManager;
    wxButton* m_btnCreateProject;
    wxButton* m_btnStopProject;
    wxButton* m_btnRun;
    wxTextCtrl* m_tcIterations;
    wxTextCtrl* m_tcNeighborhood;
    wxTextCtrl* m_tcLatticeWidth;
    wxTextCtrl* m_tcLatticeHeight;
    wxTextCtrl* m_tcMaxIterations;
    wxTextCtrl* m_tcLearningRate;
    wxTextCtrl* m_tcInitialNeighborhood;
    wxSlider* m_slider;
    wxTimer* m_updateTimer;
    bool isLatticeInitialized;
    bool isProjectStopped;

public:
    static ProjectPanel& Get(WatermarkingProject& project);
    static ProjectPanel const& Get(WatermarkingProject const& project);
    ProjectPanel(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size, WatermarkingProject& project);
    ~ProjectPanel();

private:
    void PopulateProjectPage();
    void PopulateRenderingPage();
    void OnLatticeInitialized(wxCommandEvent& evt);
    void OnButtonInitializeLattice(wxCommandEvent& event);
    void OnButtonCreateProject(wxCommandEvent& evt);
    void OnButtonStopProject(wxCommandEvent& evt);
    void OnButtonRun(wxCommandEvent& evt);
    void OnButtonWatermark(wxCommandEvent& evt);
    void OnCheckboxModel(wxCommandEvent& evt);
    void OnCheckboxLatticeVertex(wxCommandEvent& evt);
    void OnCheckboxLatticeEdge(wxCommandEvent& evt);
    void OnCheckboxLatticeFace(wxCommandEvent& evt);
    void OnCheckboxLightSource(wxCommandEvent& evt);
    void OnCheckboxLatticeFlagsCyclicX(wxCommandEvent& evt);
    void OnCheckboxLatticeFlagsCyclicY(wxCommandEvent& evt);
    void OnSliderTransparency(wxCommandEvent& evt);
    void OnTimerUIUpdate(wxTimerEvent& evt);
    void OnUpdateUI(wxUpdateUIEvent& evt);
    void OnSetLatticeWidth(wxCommandEvent& evt);
    void OnSetLatticeHeight(wxCommandEvent& evt);
    void OnSetMaxIterations(wxCommandEvent& evt);
    void OnSetLearningRate(wxCommandEvent& evt);
    void OnSetNeighborhood(wxCommandEvent& evt);
    void OnTogglePane(wxCommandEvent& event);

    WatermarkingProject& m_project;

    wxDECLARE_EVENT_TABLE();
};

wxDECLARE_EVENT(CMD_TOGGLE_RENDER_FLAG, wxCommandEvent);
wxDECLARE_EVENT(CMD_TOGGLE_LATTICE_FLAG, wxCommandEvent);

#endif
