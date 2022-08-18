#include <wx/collpane.h>

#include "Project.hpp"
#include "pane/ControlsPaneBase.hpp"

ControlsPaneBase::ControlsPaneBase(wxWindow* parent, WatermarkingProject& project)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL)
    , m_project(project)
{
    SetScrollRate(10, 10);

    SetSizer(new wxBoxSizer(wxVERTICAL));
}
