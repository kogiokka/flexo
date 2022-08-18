#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include "Project.hpp"
#include "SelfOrganizingMap.hpp"
#include "pane/WatermarkingPane.hpp"

WatermarkingPane::WatermarkingPane(wxWindow* parent, WatermarkingProject& project)
    : ControlsPaneBase(parent, project)
{
    auto* panel = CreateControlGroup("Control", 1);

    auto btnWatermark = new wxButton(panel, wxID_ANY, "Watermark");
    AppendControl(panel, "", btnWatermark);

    btnWatermark->Bind(wxEVT_UPDATE_UI,
                       [this](wxUpdateUIEvent& event) { event.Enable(SelfOrganizingMap::Get(m_project).IsDone()); });
}

void WatermarkingPane::OnWatermark(wxCommandEvent&)
{
    m_project.DoWatermark();
}
