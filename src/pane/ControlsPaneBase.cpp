#include <wx/sizer.h>

#include "Project.hpp"
#include "pane/ControlsPaneBase.hpp"

ControlsPaneBase::ControlsPaneBase(wxWindow* parent, WatermarkingProject& project)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL)
    , m_project(project)
{
    SetScrollRate(10, 10);

    SetSizer(new wxBoxSizer(wxVERTICAL));
}

ControlsGroup* ControlsPaneBase::AddGroup(wxString const& title, int numRows)
{
    auto* group = new ControlsGroup(this, title, numRows);
    GetSizer()->AddSpacer(3);
    GetSizer()->Add(group, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 15));
    return group;
}
