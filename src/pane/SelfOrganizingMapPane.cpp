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
#include "pane/ControlsGroup.hpp"
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
    auto* group = new ControlsGroup(this, "Hyperparameters", 3);

    wxIntegerValidator<int> validMaxIter;
    wxFloatingPointValidator<float> validLearnRate(6, nullptr);
    validMaxIter.SetMin(0);
    validLearnRate.SetRange(0.0f, 1.0f);

    auto* maxIterations = group->AddInputText("Max Iterations");
    auto* initLearnRate = group->AddInputText("Learning Rate");
    m_sldrNbhdRadius = group->AddSliderFloat("Neighborhood Radius", 0.0f, 0.0f, 0.0f);

    maxIterations->SetValidator(validMaxIter);
    initLearnRate->SetValidator(validLearnRate);

    *maxIterations << ProjectSettings::Get(m_project).GetMaxIterations();
    *initLearnRate << ProjectSettings::Get(m_project).GetLearningRate();

    maxIterations->Bind(wxEVT_TEXT, &SelfOrganizingMapPane::OnMaxIterations, this);
    initLearnRate->Bind(wxEVT_TEXT, &SelfOrganizingMapPane::OnInitialLearningRate, this);

    m_sldrNbhdRadius->Bind(EVT_SLIDER_FLOAT, [this](SliderFloatEvent& event) {
        ProjectSettings::Get(m_project).SetNeighborhood(event.GetValue());
    });

    GetSizer()->Add(group, wxSizerFlags().Expand());
}

void SelfOrganizingMapPane::PopulateDisplayPanel()
{
    auto* group = new ControlsGroup(this, "Display", 3);

    auto* currIterations = group->AddReadOnlyText("Iterations");
    auto* currNbhdRadius = group->AddReadOnlyText("Neighborhood Radius");
    auto* currLearnRate = group->AddReadOnlyText("Leanring Rate");

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

    GetSizer()->Add(group, wxSizerFlags().Expand());
}

void SelfOrganizingMapPane::PopulateControlPanel()
{
    auto* group = new ControlsGroup(this, "Control", 3);
    m_btnCreate = group->AddButton("Create");
    m_btnStop = group->AddButton("Stop");
    m_btnRun = group->AddButton("Run");

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

    GetSizer()->Add(group, wxSizerFlags().Expand());
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

// FIXME: Better way to do this
void SelfOrganizingMapPane::OnNeighborhoodRadiusPreset(wxCommandEvent& event)
{
    double maxRadius;
    if (event.GetString().ToDouble(&maxRadius)) {
        m_sldrNbhdRadius->SetMax(static_cast<int>(maxRadius * 100.0f));
    }

    float const radius = SelfOrganizingMap::Get(m_project).GetInitialNeighborhood();

    int const pos = static_cast<int>(radius * 100.0f);
    m_sldrNbhdRadius->SetValue(pos);

    wxCommandEvent sliderEvent(wxEVT_SLIDER);
    sliderEvent.SetInt(pos);
    m_sldrNbhdRadius->ProcessWindowEvent(sliderEvent);
}
