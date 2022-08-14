#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/valnum.h>

#include "Project.hpp"
#include "ProjectPanel.hpp"
#include "SelfOrganizingMap.hpp"
#include "SelfOrganizingMapPanel.hpp"
#include "World.hpp"

enum {
    TextCtrl_MaxIterations = wxID_HIGHEST + 100, // FIXME
    TextCtrl_InitialLearningRate,
    TextCtrl_Iterations,
    TextCtrl_LearningRate,
    TextCtrl_NeighborhoodRadius,
    Button_Create,
    Button_Stop,
    Button_Run,
    Slider_InitialNeighborhoodRadius,
};

wxBEGIN_EVENT_TABLE(SelfOrganizingMapPanel, wxScrolledWindow)
    EVT_TEXT(TextCtrl_MaxIterations, SelfOrganizingMapPanel::OnMaxIterations)
    EVT_TEXT(TextCtrl_InitialLearningRate, SelfOrganizingMapPanel::OnInitialLearningRate)
    EVT_SLIDER(Slider_InitialNeighborhoodRadius, SelfOrganizingMapPanel::OnInitialNeighborhoodRadius)
    EVT_BUTTON(Button_Run, SelfOrganizingMapPanel::OnRun)
    EVT_BUTTON(Button_Create, SelfOrganizingMapPanel::OnCreate)
    EVT_BUTTON(Button_Stop, SelfOrganizingMapPanel::OnStop)
    EVT_UPDATE_UI(Button_Run, SelfOrganizingMapPanel::OnUpdateUI)
    EVT_UPDATE_UI(TextCtrl_Iterations, SelfOrganizingMapPanel::OnUpdateUI)
    EVT_UPDATE_UI(TextCtrl_NeighborhoodRadius, SelfOrganizingMapPanel::OnUpdateUI)
    EVT_UPDATE_UI(TextCtrl_LearningRate, SelfOrganizingMapPanel::OnUpdateUI)
wxEND_EVENT_TABLE()

SelfOrganizingMapPanel::SelfOrganizingMapPanel(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size,
                                               WatermarkingProject& project)
    : wxScrolledWindow(parent, id, pos, size, wxVSCROLL)
    , m_project(project)
{
    auto layout = new wxBoxSizer(wxVERTICAL);

    wxSizerFlags flags = wxSizerFlags().Expand();
    layout->Add(PopulateParametersPanel(), flags.Border(wxTOP | wxLEFT | wxRIGHT, 10));
    layout->Add(new wxStaticLine(this), flags.Border(wxTOP | wxBOTTOM, 10));
    layout->Add(PopulateDisplayPanel(), flags.Border(wxLEFT | wxRIGHT, 10));
    layout->Add(PopulateControlPanel(), flags.Border(wxALL, 10));

    SetScrollRate(10, 10);
    SetSizer(layout);

    m_isStopped = true;

    m_project.Bind(EVT_NEIGHBORHOOD_RADIUS_PRESET, &SelfOrganizingMapPanel::OnNeighborhoodRadiusPreset, this);
}

bool SelfOrganizingMapPanel::IsProjectStopped() const
{
    return m_isStopped;
}

wxSizer* SelfOrganizingMapPanel::PopulateParametersPanel()
{
    auto paneSizer = new wxFlexGridSizer(3, 2, 5, 16);
    auto labelFlags = wxSizerFlags().Right().CenterVertical();
    auto ctrlFlags = wxSizerFlags().Expand();

    paneSizer->AddGrowableCol(0, 4);
    paneSizer->AddGrowableCol(1, 5);

    wxIntegerValidator<int> validMaxIter;
    wxFloatingPointValidator<float> validLearnRate(6, nullptr);
    validMaxIter.SetMin(0);
    validLearnRate.SetRange(0.0f, 1.0f);
    auto maxIterations = new wxTextCtrl(this, TextCtrl_MaxIterations, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                        wxTE_CENTER, validMaxIter);
    auto initLearnRate = new wxTextCtrl(this, TextCtrl_InitialLearningRate, wxEmptyString, wxDefaultPosition,
                                        wxDefaultSize, wxTE_CENTER, validLearnRate);
    m_sldrNbhdRadius = new wxSlider(this, Slider_InitialNeighborhoodRadius, 0, 0, 0, wxDefaultPosition, wxDefaultSize);
    m_nbhdRadiusText
        = new wxStaticText(this, wxID_ANY, "0.0", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);

    *maxIterations << ProjectSettings::Get(m_project).GetMaxIterations();
    *initLearnRate << ProjectSettings::Get(m_project).GetLearningRate();

    paneSizer->Add(new wxStaticText(this, wxID_ANY, "Max Iterations", wxDefaultPosition, wxDefaultSize,
                                    wxALIGN_RIGHT | wxST_ELLIPSIZE_END),
                   labelFlags);
    paneSizer->Add(maxIterations, ctrlFlags);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, "Learning Rate", wxDefaultPosition, wxDefaultSize,
                                    wxALIGN_RIGHT | wxST_ELLIPSIZE_END),
                   labelFlags);
    paneSizer->Add(initLearnRate, ctrlFlags);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, "Neighborhood Radius", wxDefaultPosition, wxDefaultSize,
                                    wxALIGN_RIGHT | wxST_ELLIPSIZE_END),
                   labelFlags);
    auto row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(m_sldrNbhdRadius, wxSizerFlags().Expand().Proportion(4));
    row->Add(m_nbhdRadiusText, wxSizerFlags().Center().Proportion(1));
    paneSizer->Add(row, ctrlFlags);

    return paneSizer;
}

wxSizer* SelfOrganizingMapPanel::PopulateDisplayPanel()
{
    auto paneSizer = new wxFlexGridSizer(3, 2, 5, 16);
    auto labelFlags = wxSizerFlags().Right().CenterVertical();
    auto ctrlFlags = wxSizerFlags().Expand();

    paneSizer->AddGrowableCol(0, 4);
    paneSizer->AddGrowableCol(1, 5);

    m_tcIterations = new wxTextCtrl(this, TextCtrl_Iterations, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                    wxTE_READONLY | wxTE_CENTER);
    m_tcNbhdRadius = new wxTextCtrl(this, TextCtrl_NeighborhoodRadius, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                    wxTE_READONLY | wxTE_CENTER);
    m_tcLearnRate = new wxTextCtrl(this, TextCtrl_LearningRate, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                   wxTE_READONLY | wxTE_CENTER);

    m_tcIterations->SetCanFocus(false);
    m_tcNbhdRadius->SetCanFocus(false);
    m_tcLearnRate->SetCanFocus(false);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, "Iterations", wxDefaultPosition, wxDefaultSize,
                                    wxALIGN_RIGHT | wxST_ELLIPSIZE_END),
                   labelFlags);
    paneSizer->Add(m_tcIterations, ctrlFlags);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, "Neighborhood Radius", wxDefaultPosition, wxDefaultSize,
                                    wxALIGN_RIGHT | wxST_ELLIPSIZE_END),
                   labelFlags);
    paneSizer->Add(m_tcNbhdRadius, ctrlFlags);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, "Learning Rate", wxDefaultPosition, wxDefaultSize,
                                    wxALIGN_RIGHT | wxST_ELLIPSIZE_END),
                   labelFlags);
    paneSizer->Add(m_tcLearnRate, ctrlFlags);

    return paneSizer;
}

wxSizer* SelfOrganizingMapPanel::PopulateControlPanel()
{
    auto paneSizer = new wxFlexGridSizer(3, 2, 5, 16);
    auto labelFlags = wxSizerFlags().Right().CenterVertical();
    auto ctrlFlags = wxSizerFlags().Expand();

    paneSizer->AddGrowableCol(0, 4);
    paneSizer->AddGrowableCol(1, 5);

    m_btnCreate = new wxButton(this, Button_Create, "Create");
    m_btnStop = new wxButton(this, Button_Stop, "Stop");
    m_btnRun = new wxButton(this, Button_Run, "Pause");

    m_btnStop->Disable();
    m_btnRun->Disable();

    paneSizer->Add(new wxStaticText(this, wxID_ANY, ""), labelFlags);
    paneSizer->Add(m_btnCreate, ctrlFlags);
    paneSizer->Add(new wxStaticText(this, wxID_ANY, ""), labelFlags);
    paneSizer->Add(m_btnStop, ctrlFlags);
    paneSizer->Add(new wxStaticText(this, wxID_ANY, ""), labelFlags);
    paneSizer->Add(m_btnRun, ctrlFlags);

    return paneSizer;
}

void SelfOrganizingMapPanel::OnCreate(wxCommandEvent&)
{
    if (!world.theModel) {
        wxMessageDialog dialog(this, "Please import a model from the File menu first.", "Error",
                               wxCENTER | wxICON_ERROR);
        dialog.ShowModal();
        return;
    }

    m_btnCreate->Disable();
    m_btnStop->Enable();
    m_btnRun->Enable();

    m_project.CreateProject();
    m_isStopped = false;
}

void SelfOrganizingMapPanel::OnStop(wxCommandEvent&)
{
    m_btnCreate->Enable();
    m_btnStop->Disable();
    m_btnRun->Disable();

    m_tcIterations->Clear();
    m_tcNbhdRadius->Clear();
    m_tcLearnRate->Clear();

    m_project.StopProject();
    m_isStopped = true;
}

void SelfOrganizingMapPanel::OnRun(wxCommandEvent&)
{
    m_btnCreate->Disable();
    m_btnStop->Enable();

    SelfOrganizingMap::Get(m_project).ToggleTraining();
}

void SelfOrganizingMapPanel::OnMaxIterations(wxCommandEvent& event)
{
    long tmp;
    if (event.GetString().ToLong(&tmp)) {
        ProjectSettings::Get(m_project).SetMaxIterations(tmp);
    }
}

void SelfOrganizingMapPanel::OnInitialLearningRate(wxCommandEvent& event)
{
    double tmp;
    if (event.GetString().ToDouble(&tmp)) {
        ProjectSettings::Get(m_project).SetLearningRate(tmp);
    }
}

void SelfOrganizingMapPanel::OnInitialNeighborhoodRadius(wxCommandEvent& event)
{
    float radius = static_cast<float>(event.GetInt()) / 100.0f;
    ProjectSettings::Get(m_project).SetNeighborhood(radius);
    m_nbhdRadiusText->SetLabelText(wxString::Format("%.2f", radius));
}

void SelfOrganizingMapPanel::OnNeighborhoodRadiusPreset(wxCommandEvent& event)
{
    // TODO: Get rid of the string indirection
    double maxRadius;
    if (event.GetString().ToDouble(&maxRadius)) {
        m_sldrNbhdRadius->SetMax(static_cast<int>(maxRadius * 100.0f));
    }

    float const radius = SelfOrganizingMap::Get(m_project).GetInitialNeighborhood();
    m_sldrNbhdRadius->SetValue(static_cast<int>(radius * 100.0f));
    m_nbhdRadiusText->SetLabelText(wxString::Format("%.2f", radius));
}

void SelfOrganizingMapPanel::OnUpdateUI(wxUpdateUIEvent& event)
{
    switch (event.GetId()) {
    case Button_Run:
        if (SelfOrganizingMap::Get(m_project).IsTraining()) {
            event.SetText("Pause");
        } else {
            event.SetText("Run");
        }
        break;
    case TextCtrl_Iterations:
        if (!m_isStopped) {
            event.SetText(wxString::Format("%d", SelfOrganizingMap::Get(m_project).GetIterations()));
        }
        break;
    case TextCtrl_NeighborhoodRadius:
        if (!m_isStopped) {
            event.SetText(wxString::Format("%.2f", SelfOrganizingMap::Get(m_project).GetNeighborhood()));
        }
        break;
    case TextCtrl_LearningRate:
        if (!m_isStopped) {
            event.SetText(wxString::Format("%.6f", SelfOrganizingMap::Get(m_project).GetLearningRate()));
        }
        break;
    default:
        break;
    }
}
