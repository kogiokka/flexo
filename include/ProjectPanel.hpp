#ifndef MAIN_PANEL_H
#define MAIN_PANEL_H

#include <wx/aui/framemanager.h>
#include <wx/event.h>
#include <wx/notebook.h>
#include <wx/window.h>

wxWindowIDRef const Panel_Notebook = wxWindow::NewControlId();

class WatermarkingProject;

class ProjectPanel : public wxNotebook
{
    wxAuiManager m_auiManager;

public:
    static ProjectPanel& Get(WatermarkingProject& project);
    static ProjectPanel const& Get(WatermarkingProject const& project);
    ProjectPanel(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size, WatermarkingProject& project);
    ~ProjectPanel();

private:
    wxWindow* CreateScrolledPanel(wxWindow* parent, wxWindowID winid);
    void PopulateProjectPage();
    void PopulateRenderingPage();
    void OnTogglePane(wxCommandEvent& event);

    WatermarkingProject& m_project;
};

#endif
