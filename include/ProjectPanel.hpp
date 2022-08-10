#ifndef MAIN_PANEL_H
#define MAIN_PANEL_H

#include <memory>

#include <wx/aui/framemanager.h>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/window.h>

#include "SelfOrganizingMapPanel.hpp"

enum {
    TE_LATTICE_WIDTH = wxID_HIGHEST + 1,
    TE_LATTICE_HEIGHT,
    BTN_INITIALIZE_LATTICE,
    BTN_WATERMARK,
    CB_RENDEROPT_MODEL,
    CB_RENDEROPT_LAT_VERTEX,
    CB_RENDEROPT_LAT_EDGE,
    CB_RENDEROPT_LAT_FACE,
    CB_RENDEROPT_LIGHT_SOURCE,
    CB_LATTICE_FLAGS_CYCLIC_X,
    CB_LATTICE_FLAGS_CYCLIC_Y,
    SLIDER_TRANSPARENCY,
    PANEL_NOTEBOOK,
    PANEL_LATTICE,
    PANEL_WATERMARKING
};

wxDECLARE_EVENT(EVT_TOGGLE_RENDER_FLAG, wxCommandEvent);
wxDECLARE_EVENT(EVT_TOGGLE_LATTICE_FLAG, wxCommandEvent);

class WatermarkingProject;

class ProjectPanel : public wxNotebook
{
    wxAuiManager m_auiManager;
    wxTextCtrl* m_tcLatticeWidth;
    wxTextCtrl* m_tcLatticeHeight;
    wxSlider* m_slider;
    SelfOrganizingMapPanel* m_somPanel;
    bool isLatticeInitialized;

public:
    static ProjectPanel& Get(WatermarkingProject& project);
    static ProjectPanel const& Get(WatermarkingProject const& project);
    ProjectPanel(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size, WatermarkingProject& project);
    ~ProjectPanel();

private:
    wxWindow* CreateScrolledPanel(wxWindow* parent, wxWindowID winid);
    void PopulateProjectPage();
    void PopulateRenderingPage();
    void OnButtonInitializeLattice(wxCommandEvent& event);
    void OnButtonWatermark(wxCommandEvent& event);
    void OnCheckboxModel(wxCommandEvent& event);
    void OnCheckboxLatticeVertex(wxCommandEvent& event);
    void OnCheckboxLatticeEdge(wxCommandEvent& event);
    void OnCheckboxLatticeFace(wxCommandEvent& event);
    void OnCheckboxLightSource(wxCommandEvent& event);
    void OnCheckboxLatticeFlagsCyclicX(wxCommandEvent& event);
    void OnCheckboxLatticeFlagsCyclicY(wxCommandEvent& event);
    void OnSliderTransparency(wxCommandEvent& event);
    void OnTimerUIUpdate(wxTimerEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);
    void OnSetLatticeWidth(wxCommandEvent& event);
    void OnSetLatticeHeight(wxCommandEvent& event);
    void OnTogglePane(wxCommandEvent& event);

    WatermarkingProject& m_project;

    wxDECLARE_EVENT_TABLE();
};

#endif
