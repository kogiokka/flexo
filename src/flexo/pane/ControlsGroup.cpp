#include <wx/stattext.h>

#include "pane/ControlsGroup.hpp"

wxDEFINE_EVENT(EVT_SLIDER_FLOAT, SliderFloatEvent);

ControlsGroup::ControlsGroup(wxWindow* parent, wxString const& title, int numRows)
    : wxCollapsiblePane(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
    m_grid = new wxFlexGridSizer(numRows, 2, 3, 10);
    m_labelFlags = wxSizerFlags().Expand();
    m_ctrlFlags = wxSizerFlags().Expand();

    m_grid->AddGrowableCol(0, 4);
    m_grid->AddGrowableCol(1, 5);

    wxSizer* paneSizer = new wxBoxSizer(wxVERTICAL);
    paneSizer->Add(m_grid, wxSizerFlags().Expand().Border(wxALL, 16));
    GetPane()->SetSizer(paneSizer);
    Expand();

    wxFont font = GetFont();
    font.SetPointSize(font.GetPointSize() - 1);
    m_widgetFont = font;
    m_widgetLabelFont = font;

    Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [parent](wxCollapsiblePaneEvent&) { parent->Layout(); });
}

wxCheckBox* ControlsGroup::AddCheckBoxWithHeading(wxString const& heading, wxString const& label, bool value)
{
    wxCheckBox* checkbox = new wxCheckBox(GetPane(), wxID_ANY, label);
    checkbox->SetValue(value);
    checkbox->SetFont(m_widgetFont);
    AppendControl(heading, checkbox);
    return checkbox;
}

wxCheckBox* ControlsGroup::AddCheckBox(wxString const& label, bool value)
{
    return AddCheckBoxWithHeading("", label, value);
}

wxRadioButton* ControlsGroup::AddRadioButtonWithHeading(wxString const& heading, wxString const& label, bool value)
{
    wxRadioButton* radiobtn = new wxRadioButton(GetPane(), wxID_ANY, label);
    radiobtn->SetValue(value);
    radiobtn->SetFont(m_widgetFont);
    AppendControl(heading, radiobtn);
    return radiobtn;
}

wxRadioButton* ControlsGroup::AddRadioButton(wxString const& label, bool value)
{
    return AddRadioButtonWithHeading("", label, value);
}

wxButton* ControlsGroup::AddButton(wxString const& label)
{
    wxButton* button = new wxButton(GetPane(), wxID_ANY, label);
    button->SetFont(m_widgetFont);
    AppendControl("", button);
    return button;
}

wxTextCtrl* ControlsGroup::AddInputText(wxString const& label, wxString const& value)
{
    wxTextCtrl* text = new wxTextCtrl(GetPane(), wxID_ANY, value, wxDefaultPosition, wxDefaultSize, wxTE_CENTER);
    text->SetFont(m_widgetFont);
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
    slider->SetFont(m_widgetFont);
    text->SetFont(m_widgetFont);

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

wxComboBox* ControlsGroup::AddComboBox(wxString const& label)
{
    wxComboBox* combo = new wxComboBox(GetPane(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr,
                                       wxCB_READONLY);
    combo->SetCanFocus(false);
    combo->SetFont(m_widgetFont);

    AppendControl(label, combo);

    return combo;
}

wxBitmapComboBox* ControlsGroup::AddBitmapComboBox(wxString const& label)
{
    wxBitmapComboBox* comboBox
        = new wxBitmapComboBox(GetPane(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr);
    comboBox->SetCanFocus(false);
    comboBox->SetFont(m_widgetFont);

    AppendControl(label, comboBox);

    return comboBox;
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
    label->SetFont(m_widgetLabelFont);
    return label;
}
