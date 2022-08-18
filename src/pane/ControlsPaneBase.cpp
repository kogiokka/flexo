#include <wx/collpane.h>

#include "pane/ControlsPaneBase.hpp"
#include "Project.hpp"

ControlsPaneBase::ControlsPaneBase(wxWindow* parent, WatermarkingProject& project)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL)
    , m_project(project)
{
    m_labelFlags = wxSizerFlags().Expand().CenterVertical();
    m_ctrlFlags = wxSizerFlags().Expand();

    SetScrollRate(10, 10);

    SetSizer(new wxBoxSizer(wxVERTICAL));
}

wxWindow* ControlsPaneBase::CreateControlGroup(wxString const& title, int rows)
{
    wxCollapsiblePane* collpane
        = new wxCollapsiblePane(this, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
    wxFlexGridSizer* sizer = new wxFlexGridSizer(rows, 2, 3, 10);

    sizer->AddGrowableCol(0, 4);
    sizer->AddGrowableCol(1, 5);

    collpane->GetPane()->SetSizer(sizer);
    collpane->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent&) { this->Layout(); });

    GetSizer()->Add(collpane, wxSizerFlags().Expand().Border(wxALL, 10));

    collpane->Expand();

    return collpane->GetPane();
}

void ControlsPaneBase::AppendControl(wxWindow* parent, wxString const& labelText, wxControl* control)
{
    wxSizer* paneSizer = parent->GetSizer();
    paneSizer->Add(CreateLabel(parent, labelText), m_labelFlags);
    paneSizer->Add(control, m_ctrlFlags);
}

void ControlsPaneBase::AppendSizer(wxWindow* parent, wxString const& labelText, wxSizer* sizer)
{
    wxSizer* paneSizer = parent->GetSizer();
    paneSizer->Add(CreateLabel(parent, labelText), m_labelFlags);
    paneSizer->Add(sizer, m_ctrlFlags);
}

wxTextCtrl* ControlsPaneBase::CreateLabel(wxWindow* parent, wxString const& text)
{
    wxTextCtrl* label = new wxTextCtrl(parent, wxID_ANY, text, wxDefaultPosition, wxDefaultSize,
                                       wxBORDER_NONE | wxTE_RIGHT | wxTE_READONLY);
    label->SetBackgroundColour(parent->GetBackgroundColour());
    label->SetCanFocus(false);
    return label;
}
