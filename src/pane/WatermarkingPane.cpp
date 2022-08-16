#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include "Project.hpp"
#include "SelfOrganizingMap.hpp"
#include "pane/WatermarkingPane.hpp"

WatermarkingPane::WatermarkingPane(wxWindow* parent, WatermarkingProject& project)
    : PaneBase(parent, project)
{
    auto layout = new wxBoxSizer(wxVERTICAL);
    auto paneSizer = new wxFlexGridSizer(1, 2, 5, 16);
    auto labelFlags = wxSizerFlags().Right().CenterVertical();
    auto ctrlFlags = wxSizerFlags().Expand();

    auto btnWatermark = new wxButton(this, wxID_ANY, "Watermark");
    paneSizer->AddGrowableCol(0, 4);
    paneSizer->AddGrowableCol(1, 5);
    paneSizer->Add(new wxStaticText(this, wxID_ANY, ""), labelFlags);
    paneSizer->Add(btnWatermark, ctrlFlags);
    layout->Add(paneSizer, wxSizerFlags().Expand().Border(wxALL, 10));

    SetSizer(layout);

    btnWatermark->Bind(wxEVT_UPDATE_UI,
                       [this](wxUpdateUIEvent& event) { event.Enable(SelfOrganizingMap::Get(m_project).IsDone()); });
}

void WatermarkingPane::OnWatermark(wxCommandEvent&)
{
    m_project.DoWatermark();
}
