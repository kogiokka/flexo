#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include "WatermarkingProject.hpp"

class ProjectSettings
{
public:
    static ProjectSettings& Get(WatermarkingProject& project);
    static ProjectSettings const& Get(WatermarkingProject const& project);

    enum EventCode : int {
        EventCode_LearningRateChanged,
        EventCode_MaxIterationChanged,
        EventCode_NeighborhoodRadiusChanged,
    };

    explicit ProjectSettings(WatermarkingProject& project) = delete;
    ProjectSettings(const ProjectSettings&) = delete;
    ProjectSettings& operator=(const ProjectSettings&) = delete;

    void SetLearningRate(float rate);
    float GetLearningRate() const;

private:
    WatermarkingProject& m_project;
    float m_learningRate;
};

wxDECLARE_EVENT(EVT_PROJECT_SETTINGS_CHANGED, wxCommandEvent);

#endif
