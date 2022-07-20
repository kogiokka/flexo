#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H

#include <wx/frame.h>

class WatermarkingProject;

class ProjectWindow : public wxFrame
{
public:
    explicit ProjectWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                           WatermarkingProject& project);
    ~ProjectWindow() override;

    WatermarkingProject& GetProject();
    WatermarkingProject const& GetProject() const;

protected:
    WatermarkingProject& m_project;
};

#endif
