#include "ProjectSettings.hpp"

#include "Project.hpp"

wxDEFINE_EVENT(EVT_PROJECT_SETTINGS_CHANGED, wxCommandEvent);

void Notify(WatermarkingProject& project, ProjectSettings::EventCode code)
{
    wxCommandEvent event(EVT_PROJECT_SETTINGS_CHANGED);
    event.SetInt(static_cast<int>(code));
    project.ProcessEvent(event);
}

ProjectSettings& ProjectSettings::Get(WatermarkingProject& project)
{
    return project.m_conf;
}

ProjectSettings const& ProjectSettings::Get(WatermarkingProject const& project)
{
    return project.m_conf;
}

ProjectSettings::ProjectSettings(WatermarkingProject& project)
    : m_project(project)
{
    m_maxIterations = 50000;
    m_learningRate = 0.15f;
    m_width = 64;
    m_height = 64;
    m_flags = 0;
}

void ProjectSettings::SetMaxIterations(unsigned int iterations)
{
    if (m_maxIterations == iterations) {
        return;
    }

    m_maxIterations = iterations;
    Notify(m_project, EventCode_ProcedureChanged);
}

float ProjectSettings::GetMaxIterations() const
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

void ProjectSettings::SetLatticeWidth(int width)
{
    if (m_width == width) {
        return;
    }

    m_width = width;
    Notify(m_project, EventCode_LatticeDimensionsChanged);
}

int ProjectSettings::GetLatticeWidth() const
{
    return m_width;
}

void ProjectSettings::SetLatticeHeight(int height)
{
    if (m_height == height) {
        return;
    }

    m_height = height;
    Notify(m_project, EventCode_LatticeDimensionsChanged);
}

int ProjectSettings::GetLatticeHeight() const
{
    return m_height;
}

void ProjectSettings::ToggleLatticeFlags(int flags)
{
    m_flags ^= flags;
}

int ProjectSettings::GetLatticeFlags() const
{
    return m_flags;
}
