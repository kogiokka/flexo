#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include "Project.hpp"
#include "SelfOrganizingMap.hpp"
#include "WatermarkingPanel.hpp"

enum {
    Button_Watermark = wxID_HIGHEST + 300,
};

wxBEGIN_EVENT_TABLE(WatermarkingPanel, wxScrolledWindow)
    EVT_BUTTON(Button_Watermark, WatermarkingPanel::OnWatermark)
    EVT_UPDATE_UI(PANEL_WATERMARKING, WatermarkingPanel::OnUpdateUI)
wxEND_EVENT_TABLE()

WatermarkingPanel::WatermarkingPanel(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size,
                                     WatermarkingProject& project)
    : wxScrolledWindow(parent, id, pos, size)
    , m_project(project)
{
    auto layout = new wxBoxSizer(wxVERTICAL);
    auto paneSizer = new wxFlexGridSizer(1, 2, 5, 16);
    auto labelFlags = wxSizerFlags().Right().CenterVertical();
    auto ctrlFlags = wxSizerFlags().Expand();

    paneSizer->AddGrowableCol(0, 4);
    paneSizer->AddGrowableCol(1, 5);
    paneSizer->Add(new wxStaticText(this, wxID_ANY, ""), labelFlags);
    paneSizer->Add(new wxButton(this, Button_Watermark, "Watermark"), ctrlFlags);
    layout->Add(paneSizer, wxSizerFlags().Expand().Border(wxALL, 10));

    SetSizer(layout);
}

void WatermarkingPanel::OnWatermark(wxCommandEvent&)
{
    m_project.DoWatermark();
}

void WatermarkingPanel::OnUpdateUI(wxUpdateUIEvent& event)
{
    switch (event.GetId()) {
    case PANEL_WATERMARKING:
        event.Enable(SelfOrganizingMap::Get(m_project).IsDone());
        break;
    default:
        break;
    }
}
