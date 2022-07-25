#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H

#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>

#include "ProjectPanel.hpp"

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

    wxWindow* GetMainPage();

    WatermarkingProject& GetProject();
    WatermarkingProject const& GetProject() const;

private:
    WatermarkingProject& m_project;
    wxWindow* m_mainPage;
    wxPanel* m_mainPanel;

    void OnOpenFile(wxCommandEvent& evt);
    void OnExit(wxCommandEvent&);
    void OnMenuCameraReset(wxCommandEvent& evt);
    void OnMenuGenerateModelDome(wxCommandEvent& evt);
};

#endif