#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H

#include <wx/frame.h>
#include <wx/sizer.h>

#include "MainPanel.hpp"
#include "OpenGLCanvas.hpp"

class WatermarkingProject;

wxDECLARE_EVENT(EVT_GENERATE_MODEL_DOME, wxCommandEvent);
wxDECLARE_EVENT(EVT_IMPORT_MODEL, wxCommandEvent);

class ProjectWindow final : public wxFrame
{
public:
    static ProjectWindow& Get(WatermarkingProject& project);
    static ProjectWindow const& Get(WatermarkingProject const& project);
    explicit ProjectWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                           WatermarkingProject& project);
    ~ProjectWindow() override;

    void SetMainPanel(MainPanel* panel);
    void SetOpenGLCanvas(OpenGLCanvas* canvas);

    WatermarkingProject& GetProject();
    WatermarkingProject const& GetProject() const;

private:
    WatermarkingProject& m_project;
    wxBoxSizer* m_layout;
    OpenGLCanvas* m_canvas;

    void OnOpenFile(wxCommandEvent& evt);
    void OnExit(wxCommandEvent&);
    void OnMenuCameraReset(wxCommandEvent& evt);
    void OnMenuGenerateModelDome(wxCommandEvent& evt);
};

#endif
