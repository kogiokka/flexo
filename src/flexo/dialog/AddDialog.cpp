#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valnum.h>

#include "dialog/AddDialog.hpp"

AddDialog::AddDialog(wxWindow* parent, wxString const& title, int numRows)
    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    m_grid = new wxFlexGridSizer(numRows, 2, 3, 10);
    m_labelFlags = wxSizerFlags().Expand();
    m_ctrlFlags = wxSizerFlags().Expand();

    m_grid->AddGrowableCol(0, 4);
    m_grid->AddGrowableCol(1, 5);

    wxSizer* paneSizer = new wxBoxSizer(wxVERTICAL);
    paneSizer->Add(m_grid, wxSizerFlags().Expand().Border(wxALL, 16));

    wxFont font = GetFont();
    font.SetPointSize(font.GetPointSize() - 1);
    m_widgetFont = font;
    m_widgetLabelFont = font;

    paneSizer->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL), wxSizerFlags().Right().Border());

    SetSizer(paneSizer);

    CenterOnScreen();
}

wxTextCtrl* AddDialog::CreateLabel(wxString const& text)
{
    wxTextCtrl* label = new wxTextCtrl(this, wxID_ANY, text, wxDefaultPosition, wxDefaultSize,
                                       wxBORDER_NONE | wxTE_RIGHT | wxTE_READONLY);
    label->SetBackgroundColour(GetBackgroundColour());
    label->SetCanFocus(false);
    label->SetFont(m_widgetLabelFont);
    return label;
}

void AddDialog::AppendControl(wxString const& labelText, wxControl* control)
{
    m_grid->Add(CreateLabel(labelText), m_labelFlags);
    m_grid->Add(control, m_ctrlFlags);
}

wxTextCtrl* AddDialog::AddInputInteger(wxString const& label, int value)
{
    wxTextCtrl* text
        = new wxTextCtrl(this, wxID_ANY, wxString() << value, wxDefaultPosition, wxDefaultSize, wxTE_CENTER);

    wxIntegerValidator<int> valid;
    valid.SetMin(1);
    text->SetValidator(valid);
    text->SetFont(m_widgetFont);

    AppendControl(label, text);

    return text;
}

wxTextCtrl* AddDialog::AddInputFloat(wxString const& label, float value)
{
    wxTextCtrl* text = new wxTextCtrl(this, wxID_ANY, wxString::Format("%.3f", value), wxDefaultPosition, wxDefaultSize,
                                      wxTE_CENTER);

    wxFloatingPointValidator<float> valid(3, nullptr);
    valid.SetMin(0.0f);
    text->SetValidator(valid);
    text->SetFont(m_widgetFont);

    AppendControl(label, text);

    return text;
}

wxCheckBox* AddDialog::AddCheckBoxWithHeading(wxString const& heading, wxString const& label, bool value)
{
    wxCheckBox* checkbox = new wxCheckBox(this, wxID_ANY, label);
    checkbox->SetValue(value);
    checkbox->SetFont(m_widgetFont);
    AppendControl(heading, checkbox);
    return checkbox;
}

wxCheckBox* AddDialog::AddCheckBox(wxString const& label, bool value)
{
    return AddCheckBoxWithHeading("", label, value);
}

wxRadioButton* AddDialog::AddRadioButtonWithHeading(wxString const& heading, wxString const& label, bool value)
{
    wxRadioButton* radiobtn = new wxRadioButton(this, wxID_ANY, label);
    radiobtn->SetValue(value);
    radiobtn->SetFont(m_widgetFont);
    AppendControl(heading, radiobtn);
    return radiobtn;
}

wxRadioButton* AddDialog::AddRadioButton(wxString const& label, bool value)
{
    return AddRadioButtonWithHeading("", label, value);
}
