#include "ProjectSettings.hpp"

#include "Project.hpp"

wxDEFINE_EVENT(EVT_PROJECT_SETTINGS_CHANGED, wxCommandEvent);

// Register factory: ProjectSettings
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<ProjectSettings> {
        auto result = std::make_shared<ProjectSettings>(project);
        return result;
    }
};

void Notify(WatermarkingProject& project, ProjectSettings::EventCode code)
{
    wxCommandEvent event(EVT_PROJECT_SETTINGS_CHANGED);
    event.SetInt(static_cast<int>(code));
    project.ProcessEvent(event);
}

ProjectSettings& ProjectSettings::Get(WatermarkingProject& project)
{
    return project.AttachedObjects::Get<ProjectSettings>(factoryKey);
}

ProjectSettings const& ProjectSettings::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

ProjectSettings::ProjectSettings(WatermarkingProject& project)
    : m_project(project)
{
    m_maxIterations = 50000;
    m_learningRate = 0.15f;
    m_neighborhood = 0.0f;
}

void ProjectSettings::SetMaxIterations(unsigned int iterations)
{
    if (m_maxIterations == iterations) {
        return;
    }

    m_maxIterations = iterations;
    Notify(m_project, EventCode_ProcedureChanged);
}

long int ProjectSettings::GetMaxIterations() const
{
    return m_maxIterations;
}

void ProjectSettings::SetNeighborhood(float radius)
{
    if (m_neighborhood == radius) {
        return;
    }

    m_neighborhood = radius;
    Notify(m_project, EventCode_ProcedureChanged);
}

float ProjectSettings::GetNeighborhood() const
{
    return m_neighborhood;
}

void ProjectSettings::SetLearningRate(float rate)
{
    if (m_learningRate == rate) {
        return;
    }

    m_learningRate = rate;
    Notify(m_project, EventCode_ProcedureChanged);
}

float ProjectSettings::GetLearningRate() const
{
    return m_learningRate;
}
