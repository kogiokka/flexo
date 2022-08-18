#include <wx/collpane.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/valnum.h>

#include "Project.hpp"
#include "SelfOrganizingMap.hpp"
#include "World.hpp"
#include "pane/SelfOrganizingMapPane.hpp"

SelfOrganizingMapPane::SelfOrganizingMapPane(wxWindow* parent, WatermarkingProject& project)
    : ControlsPaneBase(parent, project)
{
    PopulateParametersPanel();
    PopulateDisplayPanel();
    PopulateControlPanel();

    m_isStopped = true;

    m_project.Bind(EVT_NEIGHBORHOOD_RADIUS_PRESET, &SelfOrganizingMapPane::OnNeighborhoodRadiusPreset, this);
}

bool SelfOrganizingMapPane::IsProjectStopped() const
{
    return m_isStopped;
}

void SelfOrganizingMapPane::PopulateParametersPanel()
{
    wxWindow* panel = CreateControlGroup("Hyperparameters", 3);

    wxIntegerValidator<int> validMaxIter;
    wxFloatingPointValidator<float> validLearnRate(6, nullptr);
    validMaxIter.SetMin(0);
    validLearnRate.SetRange(0.0f, 1.0f);
    auto maxIterations
        = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER, validMaxIter);
    auto initLearnRate
        = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER, validLearnRate);

    m_sldrNbhdRadius = new wxSlider(panel, wxID_ANY, 0, 0, 0, wxDefaultPosition, wxDefaultSize);
    m_nbhdRadiusText
        = new wxStaticText(panel, wxID_ANY, "0.0", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
    auto row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(m_sldrNbhdRadius, wxSizerFlags().Expand().Proportion(4));
    row->Add(m_nbhdRadiusText, wxSizerFlags().Center().Proportion(1));

    *maxIterations << ProjectSettings::Get(m_project).GetMaxIterations();
    *initLearnRate << ProjectSettings::Get(m_project).GetLearningRate();

    AppendControl(panel, "Max Iterations", maxIterations);
    AppendControl(panel, "Leanring Rate", initLearnRate);
    AppendSizer(panel, "Neighborhood Radius", row);

    maxIterations->Bind(wxEVT_TEXT, &SelfOrganizingMapPane::OnMaxIterations, this);
    initLearnRate->Bind(wxEVT_TEXT, &SelfOrganizingMapPane::OnInitialLearningRate, this);
    m_sldrNbhdRadius->Bind(wxEVT_SLIDER, &SelfOrganizingMapPane::OnInitialNeighborhoodRadius, this);
}

void SelfOrganizingMapPane::PopulateDisplayPanel()
{
    auto* panel = CreateControlGroup("Display", 3);

    auto currIterations
        = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
    auto currNbhdRadius
        = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
    auto currLearnRate
        = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
    currIterations->SetCanFocus(false);
    currNbhdRadius->SetCanFocus(false);
    currLearnRate->SetCanFocus(false);

    AppendControl(panel, "Iterations", currIterations);
    AppendControl(panel, "Neighborhood Radius", currNbhdRadius);
    AppendControl(panel, "Leanring Rate", currLearnRate);

    currIterations->Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) {
        if (m_isStopped) {
            event.SetText("");
        } else {
            event.SetText(wxString::Format("%d", SelfOrganizingMap::Get(m_project).GetIterations()));
        }
    });

    currNbhdRadius->Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) {
        if (m_isStopped) {
            event.SetText("");
        } else {
            event.SetText(wxString::Format("%.2f", SelfOrganizingMap::Get(m_project).GetNeighborhood()));
        }
    });

    currLearnRate->Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) {
        if (m_isStopped) {
            event.SetText("");
        } else {
            event.SetText(wxString::Format("%.6f", SelfOrganizingMap::Get(m_project).GetLearningRate()));
        }
    });
}

void SelfOrganizingMapPane::PopulateControlPanel()
{
    auto* panel = CreateControlGroup("Control", 3);

    m_btnCreate = new wxButton(panel, wxID_ANY, "Create");
    m_btnStop = new wxButton(panel, wxID_ANY, "Stop");
    m_btnRun = new wxButton(panel, wxID_ANY, "Pause");

    m_btnStop->Disable();
    m_btnRun->Disable();

    m_btnRun->Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) {
        if (SelfOrganizingMap::Get(m_project).IsTraining()) {
            event.SetText("Pause");
        } else {
            event.SetText("Run");
        }
    });

    m_btnCreate->Bind(wxEVT_BUTTON, &SelfOrganizingMapPane::OnCreate, this);
    m_btnStop->Bind(wxEVT_BUTTON, &SelfOrganizingMapPane::OnStop, this);
    m_btnRun->Bind(wxEVT_BUTTON, &SelfOrganizingMapPane::OnRun, this);

    AppendControl(panel, "", m_btnCreate);
    AppendControl(panel, "", m_btnStop);
    AppendControl(panel, "", m_btnRun);
}

void SelfOrganizingMapPane::OnCreate(wxCommandEvent&)
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

void SelfOrganizingMapPane::OnStop(wxCommandEvent&)
{
    m_btnCreate->Enable();
    m_btnStop->Disable();
    m_btnRun->Disable();

    m_project.StopProject();
    m_isStopped = true;
}

void SelfOrganizingMapPane::OnRun(wxCommandEvent&)
{
    m_btnCreate->Disable();
    m_btnStop->Enable();

    SelfOrganizingMap::Get(m_project).ToggleTraining();
}

void SelfOrganizingMapPane::OnMaxIterations(wxCommandEvent& event)
{
    long tmp;
    if (event.GetString().ToLong(&tmp)) {
        ProjectSettings::Get(m_project).SetMaxIterations(tmp);
    }
}

void SelfOrganizingMapPane::OnInitialLearningRate(wxCommandEvent& event)
{
    double tmp;
    if (event.GetString().ToDouble(&tmp)) {
        ProjectSettings::Get(m_project).SetLearningRate(tmp);
    }
}

void SelfOrganizingMapPane::OnInitialNeighborhoodRadius(wxCommandEvent& event)
{
    float radius = static_cast<float>(event.GetInt()) / 100.0f;
    ProjectSettings::Get(m_project).SetNeighborhood(radius);
    m_nbhdRadiusText->SetLabelText(wxString::Format("%.2f", radius));
}

void SelfOrganizingMapPane::OnNeighborhoodRadiusPreset(wxCommandEvent& event)
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
