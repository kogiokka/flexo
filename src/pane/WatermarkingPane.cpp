#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include "Project.hpp"
#include "SelfOrganizingMap.hpp"
#include "pane/WatermarkingPane.hpp"

WatermarkingPane::WatermarkingPane(wxWindow* parent, WatermarkingProject& project)
    : ControlsPaneBase(parent, project)
{
    auto* group = AddGroup("Control", 1);
    group->AddButton("Watermark");

    Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) { event.Enable(SelfOrganizingMap::Get(m_project).IsDone()); });
}

void WatermarkingPane::OnWatermark(wxCommandEvent&)
{
    m_project.DoWatermark();
}
