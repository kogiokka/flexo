#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include "Project.hpp"
#include "SelfOrganizingMap.hpp"
#include "pane/ControlsGroup.hpp"
#include "pane/WatermarkingPane.hpp"

WatermarkingPane::WatermarkingPane(wxWindow* parent, WatermarkingProject& project)
    : ControlsPaneBase(parent, project)
{
    auto* group = new ControlsGroup(this, "Control", 1);
    auto* btnWatermark = group->AddButton("Watermark");
    btnWatermark->Bind(wxEVT_UPDATE_UI,
                       [this](wxUpdateUIEvent& event) { event.Enable(SelfOrganizingMap::Get(m_project).IsDone()); });

    GetSizer()->Add(group, wxSizerFlags().Expand());
}

void WatermarkingPane::OnWatermark(wxCommandEvent&)
{
    m_project.DoWatermark();
}
