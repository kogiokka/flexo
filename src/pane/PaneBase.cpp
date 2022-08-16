#include "pane/PaneBase.hpp"
#include "Project.hpp"

PaneBase::PaneBase(wxWindow* parent, WatermarkingProject& project)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL)
    , m_project(project)
{
    SetScrollRate(10, 10);
}
