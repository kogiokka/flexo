#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include <wx/event.h>

#include "Attachable.hpp"

class WatermarkingProject;

wxDECLARE_EVENT(EVT_PROJECT_SETTINGS_CHANGED, wxCommandEvent);

class ProjectSettings : public AttachableBase
{
public:
    static ProjectSettings& Get(WatermarkingProject& project);
    static ProjectSettings const& Get(WatermarkingProject const& project);

    enum EventCode : int {
        EventCode_ProcedureChanged,
    };

    explicit ProjectSettings(WatermarkingProject& project);
    virtual ~ProjectSettings() {};
    ProjectSettings(ProjectSettings const&) = delete;
    ProjectSettings& operator=(ProjectSettings const&) = delete;

    void SetMaxIterations(unsigned int iterations);
    long int GetMaxIterations() const;
    void SetNeighborhood(float rate);
    float GetNeighborhood() const;
    void SetLearningRate(float rate);
    float GetLearningRate() const;

private:
    WatermarkingProject& m_project;
    float m_learningRate;
    long int m_maxIterations;
    float m_neighborhood;
};

#endif
