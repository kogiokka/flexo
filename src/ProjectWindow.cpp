#include "ProjectWindow.hpp"
#include "Project.hpp"

ProjectWindow::ProjectWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                             WatermarkingProject& project)
    : wxFrame(parent, id, "Self-organizing Map Demo", pos, size)
    , m_project(project)
{
    m_project.SetFrame(this);
}

WatermarkingProject& ProjectWindow::GetProject()
{
    return m_project;
}

WatermarkingProject const& ProjectWindow::GetProject() const
{
    return m_project;
}
