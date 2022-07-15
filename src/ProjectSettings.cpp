#include "ProjectSettings.hpp"

wxDEFINE_EVENT(EVT_PROJECT_SETTINGS_CHANGED, wxCommandEvent);

void Notify(WatermarkingProject& project, ProjectSettings::EventCode code)
{
    wxCommandEvent event { EVT_PROJECT_SETTINGS_CHANGED };
    event.SetInt(static_cast<int>(code));
    project.ProcessEvent(event);
}

void ProjectSettings::SetLearningRate(float rate)
{
    m_learningRate = rate;
    Notify(m_project, EventCode_LearningRateChanged);
}

float ProjectSettings::GetLearningRate() const
{
    return m_learningRate;
}
