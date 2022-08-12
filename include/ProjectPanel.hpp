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
    BTN_WATERMARK = wxID_HIGHEST + 1,
    CB_RENDEROPT_MODEL,
    CB_RENDEROPT_LAT_VERTEX,
    CB_RENDEROPT_LAT_EDGE,
    CB_RENDEROPT_LAT_FACE,
    CB_RENDEROPT_LIGHT_SOURCE,
    SLIDER_TRANSPARENCY,
    PANEL_NOTEBOOK,
    PANEL_WATERMARKING
};

wxDECLARE_EVENT(EVT_TOGGLE_RENDER_FLAG, wxCommandEvent);

class WatermarkingProject;

class ProjectPanel : public wxNotebook
{
    wxAuiManager m_auiManager;
    wxSlider* m_slider;
    SelfOrganizingMapPanel* m_somPanel;

public:
    static ProjectPanel& Get(WatermarkingProject& project);
    static ProjectPanel const& Get(WatermarkingProject const& project);
    ProjectPanel(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size, WatermarkingProject& project);
    ~ProjectPanel();

private:
    wxWindow* CreateScrolledPanel(wxWindow* parent, wxWindowID winid);
    void PopulateProjectPage();
    void PopulateRenderingPage();
    void OnButtonWatermark(wxCommandEvent& event);
    void OnCheckboxModel(wxCommandEvent& event);
    void OnCheckboxLatticeVertex(wxCommandEvent& event);
    void OnCheckboxLatticeEdge(wxCommandEvent& event);
    void OnCheckboxLatticeFace(wxCommandEvent& event);
    void OnCheckboxLightSource(wxCommandEvent& event);
    void OnSliderTransparency(wxCommandEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);
    void OnTogglePane(wxCommandEvent& event);

    WatermarkingProject& m_project;

    wxDECLARE_EVENT_TABLE();
};

#endif
