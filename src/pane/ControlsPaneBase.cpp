#include "pane/ControlsPaneBase.hpp"
#include "Project.hpp"

ControlsPaneBase::ControlsPaneBase(wxWindow* parent, WatermarkingProject& project)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL)
    , m_project(project)
{
    m_labelFlags = wxSizerFlags().Expand().CenterVertical();
    m_ctrlFlags = wxSizerFlags().Expand();

    SetScrollRate(10, 10);
}

wxFlexGridSizer* ControlsPaneBase::CreateGroup(int rows)
{
    wxFlexGridSizer* sizer = new wxFlexGridSizer(rows, 2, 5, 16);
    sizer->AddGrowableCol(0, 4);
    sizer->AddGrowableCol(1, 5);

    return sizer;
}

wxTextCtrl* ControlsPaneBase::CreateLabel(wxString const& text)
{
    wxTextCtrl* label = new wxTextCtrl(this, wxID_ANY, text, wxDefaultPosition, wxDefaultSize,
                                       wxBORDER_NONE | wxTE_RIGHT | wxTE_READONLY);
    label->SetBackgroundColour(GetBackgroundColour());
    label->SetCanFocus(false);
    return label;
}

void ControlsPaneBase::AppendControl(wxFlexGridSizer* group, wxString const& labelText, wxControl* control)
{
    group->Add(CreateLabel(labelText), m_labelFlags);
    group->Add(control, m_ctrlFlags);
}

void ControlsPaneBase::AppendSizer(wxFlexGridSizer* group, wxString const& labelText, wxSizer* sizer)
{
    group->Add(CreateLabel(labelText), m_labelFlags);
    group->Add(sizer, m_ctrlFlags);
}
