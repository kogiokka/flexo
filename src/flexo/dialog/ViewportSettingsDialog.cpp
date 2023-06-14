#include <wx/checkbox.h>
#include <wx/textctrl.h>

#include "Project.hpp"
#include "dialog/ViewportSettingsDialog.hpp"

wxDEFINE_EVENT(EVT_VIEWPORT_SETTINGS_DISPLAY_GUIDES, wxCommandEvent);

ViewportSettingsDialog::ViewportSettingsDialog(wxWindow* parent, SceneViewportPane::Settings settings,
                                               FlexoProject& project)
    : wxDialog(parent, wxID_ANY, "Viewport Settings", wxDefaultPosition, wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    , m_project(project)
{
    wxTextCtrl* labelGuides;
    wxCheckBox* cbGuides;
    wxStdDialogButtonSizer* stdbtn;

    labelGuides = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                                 wxBORDER_NONE | wxTE_RIGHT | wxTE_READONLY);

    auto bg = this->GetBackgroundColour();
    labelGuides->SetBackgroundColour(bg);
    labelGuides->SetCanFocus(false);

    cbGuides = new wxCheckBox(this, wxID_ANY, "Display Guides");
    cbGuides->SetValue((settings.overlayFlags) != 0);

    stdbtn = CreateStdDialogButtonSizer(wxOK);

    // Events
    cbGuides->Bind(wxEVT_CHECKBOX, &ViewportSettingsDialog::OnGuidesCheckbox, this);

    // Sizers
    wxFlexGridSizer* grid = new wxFlexGridSizer(2, wxSize(10, 3));
    grid->AddGrowableCol(0, 4);
    grid->AddGrowableCol(1, 5);

    grid->Add(labelGuides, wxSizerFlags().Expand().Proportion(4));
    grid->Add(cbGuides, wxSizerFlags().Expand().Proportion(5));

    m_topLayout = new wxBoxSizer(wxVERTICAL);
    m_topLayout->Add(grid, wxSizerFlags().Expand().Border(wxALL, 16));
    m_topLayout->Add(stdbtn, wxSizerFlags().Right().Border(wxALL, 16));
    m_topLayout->SetMinSize(GetSize());
    m_topLayout->SetSizeHints(this);

    SetAutoLayout(true);
    SetSizer(m_topLayout);
    CenterOnParent();
}

void ViewportSettingsDialog::OnGuidesCheckbox(wxCommandEvent& event)
{
    wxCommandEvent signal(EVT_VIEWPORT_SETTINGS_DISPLAY_GUIDES);
    signal.SetInt(event.GetInt());
    m_project.ProcessEvent(signal);
}
