#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/treectrl.h>

#include "SceneOutlinerPanel.hpp"
#include "World.hpp"

#include "common/Logger.hpp"

wxDEFINE_EVENT(EVT_TOGGLE_RENDER_FLAG, wxCommandEvent);

SceneOutlinerPanel::SceneOutlinerPanel(wxWindow* parent, WatermarkingProject& project)
    : PaneBase(parent, project)
{
    auto layout = new wxBoxSizer(wxVERTICAL);
    auto paneSizer = new wxFlexGridSizer(6, 2, 5, 16);
    auto labelFlags = wxSizerFlags().Expand();
    auto ctrlFlags = wxSizerFlags().Expand();

    paneSizer->AddGrowableCol(0, 4);
    paneSizer->AddGrowableCol(1, 5);

    auto chkBox1 = new wxCheckBox(this, wxID_ANY, "Model");
    auto chkBox2 = new wxCheckBox(this, wxID_ANY, "Lattice Vertex");
    auto chkBox3 = new wxCheckBox(this, wxID_ANY, "Lattice Edge");
    auto chkBox4 = new wxCheckBox(this, wxID_ANY, "Lattice Face");
    auto chkBox5 = new wxCheckBox(this, wxID_ANY, "Light Source");
    chkBox1->SetValue(true);
    chkBox2->SetValue(true);
    chkBox3->SetValue(true);
    chkBox4->SetValue(false);
    chkBox5->SetValue(false);

    // Trigger the event to update render flags.
    wxCommandEvent event;
    OnCheckboxModel(event);
    OnCheckboxLatticeVertex(event);
    OnCheckboxLatticeEdge(event);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, ""), labelFlags);
    paneSizer->Add(chkBox1, ctrlFlags);
    paneSizer->Add(new wxStaticText(this, wxID_ANY, ""), labelFlags);
    paneSizer->Add(chkBox2, ctrlFlags);
    paneSizer->Add(new wxStaticText(this, wxID_ANY, ""), labelFlags);
    paneSizer->Add(chkBox3, ctrlFlags);
    paneSizer->Add(new wxStaticText(this, wxID_ANY, ""), labelFlags);
    paneSizer->Add(chkBox4, ctrlFlags);
    paneSizer->Add(new wxStaticText(this, wxID_ANY, ""), labelFlags);
    paneSizer->Add(chkBox5, ctrlFlags);

    auto transparencyLabel = new wxTextCtrl(this, wxID_ANY, "Model Transparency (%)", wxDefaultPosition, wxDefaultSize,
                                            wxBORDER_NONE | wxTE_READONLY | wxTE_MULTILINE | wxTE_CHARWRAP);
    transparencyLabel->SetCanFocus(false);

    int const sliderInit = static_cast<int>(100.0f - world.modelColorAlpha * 100.0f);
    m_slider = new wxSlider(this, wxID_ANY, sliderInit, 0, 100, wxDefaultPosition, wxDefaultSize,
                            wxSL_HORIZONTAL | wxSL_LABELS);
    paneSizer->Add(transparencyLabel, labelFlags);
    paneSizer->Add(m_slider, ctrlFlags);
    layout->Add(paneSizer, wxSizerFlags().Expand().Border(wxALL, 10));

    SetSizer(layout);

    chkBox1->Bind(wxEVT_CHECKBOX, &SceneOutlinerPanel::OnCheckboxModel, this);
    chkBox2->Bind(wxEVT_CHECKBOX, &SceneOutlinerPanel::OnCheckboxLatticeVertex, this);
    chkBox3->Bind(wxEVT_CHECKBOX, &SceneOutlinerPanel::OnCheckboxLatticeEdge, this);
    chkBox4->Bind(wxEVT_CHECKBOX, &SceneOutlinerPanel::OnCheckboxLatticeFace, this);
    chkBox5->Bind(wxEVT_CHECKBOX, &SceneOutlinerPanel::OnCheckboxLightSource, this);
    m_slider->Bind(wxEVT_SLIDER, &SceneOutlinerPanel::OnSliderTransparency, this);
}

void SceneOutlinerPanel::OnCheckboxModel(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawModel);
    ProcessWindowEvent(event);
}

void SceneOutlinerPanel::OnCheckboxLatticeVertex(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeVertex);
    ProcessWindowEvent(event);
}

void SceneOutlinerPanel::OnCheckboxLatticeEdge(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeEdge);
    ProcessWindowEvent(event);
}

void SceneOutlinerPanel::OnCheckboxLatticeFace(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeFace);
    ProcessWindowEvent(event);
}

void SceneOutlinerPanel::OnCheckboxLightSource(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLightSource);
    ProcessWindowEvent(event);
}

void SceneOutlinerPanel::OnSliderTransparency(wxCommandEvent&)
{
    float const range = static_cast<float>(m_slider->GetMax() - m_slider->GetMin());
    float const value = static_cast<float>(m_slider->GetValue());
    world.modelColorAlpha = (range - value) / range;
}
