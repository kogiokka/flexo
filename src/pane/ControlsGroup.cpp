#include <wx/stattext.h>

#include "pane/ControlsGroup.hpp"

wxDEFINE_EVENT(EVT_SLIDER_FLOAT, SliderFloatEvent);

ControlsGroup::ControlsGroup(wxWindow* parent, wxString const& title, int rows)
    : wxCollapsiblePane(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
    m_grid = new wxFlexGridSizer(rows, 2, 3, 10);
    m_labelFlags = wxSizerFlags().Expand();
    m_ctrlFlags = wxSizerFlags().Expand();

    m_grid->AddGrowableCol(0, 4);
    m_grid->AddGrowableCol(1, 5);

    wxSizer* paneSizer = new wxBoxSizer(wxVERTICAL);
    paneSizer->Add(m_grid, wxSizerFlags().Expand().Border(wxALL, 16));
    GetPane()->SetSizer(paneSizer);
    Expand();

    Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [parent](wxCollapsiblePaneEvent&) { parent->Layout(); });
}

wxCheckBox* ControlsGroup::AddCheckBox(wxString const& label, bool value)
{
    wxCheckBox* checkbox = new wxCheckBox(GetPane(), wxID_ANY, label);
    checkbox->SetValue(value);
    AppendControl("", checkbox);
    return checkbox;
}

wxButton* ControlsGroup::AddButton(wxString const& label)
{
    wxButton* button = new wxButton(GetPane(), wxID_ANY, label);
    AppendControl("", button);
    return button;
}

wxTextCtrl* ControlsGroup::AddInputText(wxString const& label, wxString const& value)
{
    wxTextCtrl* text = new wxTextCtrl(GetPane(), wxID_ANY, value, wxDefaultPosition, wxDefaultSize, wxTE_CENTER);
    AppendControl(label, text);
    return text;
}

wxTextCtrl* ControlsGroup::AddReadOnlyText(wxString const& label, wxString const& value)
{
    wxTextCtrl* text
        = new wxTextCtrl(GetPane(), wxID_ANY, value, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
    text->SetCanFocus(false);
    AppendControl(label, text);
    return text;
}

wxSlider* ControlsGroup::AddSliderFloat(wxString const& label, float value, float minValue, float maxValue)
{
    float const scale = 100.0f;

    wxWindow* pane = GetPane();
    wxSlider* slider = new wxSlider(pane, wxID_ANY, static_cast<int>(value * scale), static_cast<int>(minValue * scale),
                                    static_cast<int>(maxValue * scale));
    wxStaticText* text = new wxStaticText(pane, wxID_ANY, wxString::Format("%.2f", value), wxDefaultPosition,
                                          wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);

    slider->Bind(wxEVT_SLIDER, [this, text, slider](wxCommandEvent& event) {
        float const value = event.GetInt() * 0.01f;
        text->SetLabelText(wxString::Format("%.2f", value));
        SliderFloatEvent floatEvent(EVT_SLIDER_FLOAT, this->GetId(), value);
        slider->ProcessWindowEvent(floatEvent);
    });

    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(slider, wxSizerFlags().Expand().Proportion(4));
    sizer->Add(text, wxSizerFlags().Center().Proportion(1));

    AppendSizer(label, sizer);

    return slider;
}

void ControlsGroup::AppendControl(wxString const& labelText, wxControl* control)
{
    m_grid->Add(CreateLabel(labelText), m_labelFlags);
    m_grid->Add(control, m_ctrlFlags);
}

void ControlsGroup::AppendSizer(wxString const& labelText, wxSizer* sizer)
{
    m_grid->Add(CreateLabel(labelText), m_labelFlags);
    m_grid->Add(sizer, m_ctrlFlags);
}

wxTextCtrl* ControlsGroup::CreateLabel(wxString const& text)
{
    wxWindow* pane = GetPane();
    wxTextCtrl* label = new wxTextCtrl(pane, wxID_ANY, text, wxDefaultPosition, wxDefaultSize,
                                       wxBORDER_NONE | wxTE_RIGHT | wxTE_READONLY);
    label->SetBackgroundColour(pane->GetBackgroundColour());
    label->SetCanFocus(false);
    return label;
}
