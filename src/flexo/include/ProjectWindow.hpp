#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H

#include <wx/aui/framemanager.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/timer.h>

class FlexoProject;

wxDECLARE_EVENT(EVT_OPEN_MODEL, wxCommandEvent);
wxDECLARE_EVENT(EVT_SCREENSHOT, wxCommandEvent);
wxDECLARE_EVENT(EVT_IMPORT_MODEL, wxCommandEvent);

wxDECLARE_EVENT(EVT_MENU_CAMERA_PERSPECTIVE, wxCommandEvent);
wxDECLARE_EVENT(EVT_MENU_CAMERA_ORTHOGONAL, wxCommandEvent);

class ProjectWindow final : public wxFrame
{
public:
    static ProjectWindow& Get(FlexoProject& project);
    static ProjectWindow const& Get(FlexoProject const& project);
    explicit ProjectWindow(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size,
                           FlexoProject& project);
    ~ProjectWindow() override;

    wxAuiManager& GetPaneManager();
    wxWindow* GetMainPage();

    FlexoProject& GetProject();
    FlexoProject const& GetProject() const;

private:
    FlexoProject& m_project;
    wxWindow* m_mainPage;
    wxPanel* m_mainPanel;
    wxTimer* m_updateUITimer;
    wxMenu* m_viewMenu;
    wxAuiManager m_mgr;

    void OnOpenModelFile(wxCommandEvent& event);
    void OnExit(wxCommandEvent&);
    void OnTimerUpdateUI(wxTimerEvent& event);
    void OnTogglePane(wxCommandEvent& event);
    void OnViewportSettings(wxCommandEvent& event);
};

#endif
