#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include <wx/event.h>

class WatermarkingProject;

wxDECLARE_EVENT(EVT_PROJECT_SETTINGS_CHANGED, wxCommandEvent);

class ProjectSettings
{
public:
    static ProjectSettings& Get(WatermarkingProject& project);
    static ProjectSettings const& Get(WatermarkingProject const& project);

    enum EventCode : int {
        EventCode_LatticeDimensionsChanged,
        EventCode_LatticeFlagsChanged,
        EventCode_LearningRateChanged,
        EventCode_MaxIterationChanged,
        EventCode_NeighborhoodRadiusChanged,
    };

    explicit ProjectSettings(WatermarkingProject& project);
    ProjectSettings(const ProjectSettings&) = delete;
    ProjectSettings& operator=(const ProjectSettings&) = delete;

    void SetMaxIterations(unsigned int iterations);
    float GetMaxIterations() const;
    void SetNeighborhood(float rate);
    float GetNeighborhood() const;
    void SetLearningRate(float rate);
    float GetLearningRate() const;
    void SetLatticeWidth(int width);
    int GetLatticeWidth() const;
    void SetLatticeHeight(int height);
    int GetLatticeHeight() const;
    void SetLatticeFlags(int flags);
    int GetLatticeFlags() const;

private:
    WatermarkingProject& m_project;
    float m_learningRate;
    int m_width;
    int m_height;
    unsigned int m_maxIterations;
    float m_initialNeighborhood;
    int m_flags;
};

#endif
