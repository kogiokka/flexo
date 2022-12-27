#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H

#include <wx/aui/framemanager.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/timer.h>

class WatermarkingProject;

#define MENU_ADD_LIST                                                                                                  \
    X(EVT_MENU_ADD_PLANE, "Plane")                                                                                     \
    X(EVT_MENU_ADD_GRID, "Grid")                                                                                       \
    X(EVT_MENU_ADD_CUBE, "Cube")                                                                                       \
    X(EVT_MENU_ADD_UV_SPHERE, "UV Sphere")                                                                             \
    X(EVT_MENU_ADD_TORUS, "Torus")                                                                                     \
    X(EVT_MENU_ADD_MAP, "Map (3 to 2)")

wxDECLARE_EVENT(EVT_OPEN_MODEL, wxCommandEvent);
wxDECLARE_EVENT(EVT_OPEN_IMAGE, wxCommandEvent);
wxDECLARE_EVENT(EVT_SCREENSHOT, wxCommandEvent);
wxDECLARE_EVENT(EVT_IMPORT_MODEL, wxCommandEvent);

#define X(evt, name) wxDECLARE_EVENT(evt, wxCommandEvent);
MENU_ADD_LIST
#undef X

wxDECLARE_EVENT(EVT_MENU_CAMERA_PERSPECTIVE, wxCommandEvent);
wxDECLARE_EVENT(EVT_MENU_CAMERA_ORTHOGONAL, wxCommandEvent);

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

    void OnOpenModelFile(wxCommandEvent& event);
    void OnOpenImageFile(wxCommandEvent& event);
    void OnExit(wxCommandEvent&);
    void OnTimerUpdateUI(wxTimerEvent& event);
    void OnTogglePane(wxCommandEvent& event);
};

#endif
