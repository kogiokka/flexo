#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H

#include <wx/aui/framemanager.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/timer.h>

class WatermarkingProject;

wxDECLARE_EVENT(EVT_GENERATE_MODEL_DOME, wxCommandEvent);
wxDECLARE_EVENT(EVT_IMPORT_MODEL, wxCommandEvent);

class ProjectWindow final : public wxFrame
{
public:
    static ProjectWindow& Get(WatermarkingProject& project);
    static ProjectWindow const& Get(WatermarkingProject const& project);
    explicit ProjectWindow(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size,
                           WatermarkingProject& project);
    ~ProjectWindow() override;

    wxAuiManager& GetPaneManager();
    wxWindow* GetMainPage();

    WatermarkingProject& GetProject();
    WatermarkingProject const& GetProject() const;

private:
    WatermarkingProject& m_project;
    wxWindow* m_mainPage;
    wxPanel* m_mainPanel;
    wxTimer* m_updateUITimer;
    wxMenu* m_viewMenu;
    wxAuiManager m_mgr;

    void OnOpenFile(wxCommandEvent& event);
    void OnExit(wxCommandEvent&);
    void OnMenuCameraReset(wxCommandEvent& event);
    void OnMenuGenerateModelDome(wxCommandEvent& event);
    void OnTimerUpdateUI(wxTimerEvent& event);
    void OnTogglePane(wxCommandEvent& event);
};

#endif
