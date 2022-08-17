#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/treectrl.h>

#include "World.hpp"
#include "pane/SceneOutlinerPane.hpp"

wxDEFINE_EVENT(EVT_TOGGLE_RENDER_FLAG, wxCommandEvent);

SceneOutlinerPane::SceneOutlinerPane(wxWindow* parent, WatermarkingProject& project)
    : ControlsPaneBase(parent, project)
{
    auto layout = new wxBoxSizer(wxVERTICAL);
    auto* group = CreateGroup(6);

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

    int const sliderInit = static_cast<int>(100.0f - world.modelColorAlpha * 100.0f);
    m_slider = new wxSlider(this, wxID_ANY, sliderInit, 0, 100, wxDefaultPosition, wxDefaultSize,
                            wxSL_HORIZONTAL | wxSL_LABELS);

    AppendControl(group, "", chkBox1);
    AppendControl(group, "", chkBox2);
    AppendControl(group, "", chkBox3);
    AppendControl(group, "", chkBox4);
    AppendControl(group, "", chkBox5);
    AppendControl(group, "Model Transparency (%)", m_slider);

    layout->Add(group, wxSizerFlags().Expand().Border(wxALL, 10));
    SetSizer(layout);

    chkBox1->Bind(wxEVT_CHECKBOX, &SceneOutlinerPane::OnCheckboxModel, this);
    chkBox2->Bind(wxEVT_CHECKBOX, &SceneOutlinerPane::OnCheckboxLatticeVertex, this);
    chkBox3->Bind(wxEVT_CHECKBOX, &SceneOutlinerPane::OnCheckboxLatticeEdge, this);
    chkBox4->Bind(wxEVT_CHECKBOX, &SceneOutlinerPane::OnCheckboxLatticeFace, this);
    chkBox5->Bind(wxEVT_CHECKBOX, &SceneOutlinerPane::OnCheckboxLightSource, this);
    m_slider->Bind(wxEVT_SLIDER, &SceneOutlinerPane::OnSliderTransparency, this);
}

void SceneOutlinerPane::OnCheckboxModel(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawModel);
    ProcessWindowEvent(event);
}

void SceneOutlinerPane::OnCheckboxLatticeVertex(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeVertex);
    ProcessWindowEvent(event);
}

void SceneOutlinerPane::OnCheckboxLatticeEdge(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeEdge);
    ProcessWindowEvent(event);
}

void SceneOutlinerPane::OnCheckboxLatticeFace(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeFace);
    ProcessWindowEvent(event);
}

void SceneOutlinerPane::OnCheckboxLightSource(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLightSource);
    ProcessWindowEvent(event);
}

void SceneOutlinerPane::OnSliderTransparency(wxCommandEvent&)
{
    float const range = static_cast<float>(m_slider->GetMax() - m_slider->GetMin());
    float const value = static_cast<float>(m_slider->GetValue());
    world.modelColorAlpha = (range - value) / range;
}
