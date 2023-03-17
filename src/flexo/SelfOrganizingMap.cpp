#include "SelfOrganizingMap.hpp"
#include "Project.hpp"
#include "ProjectSettings.hpp"
#include "object/Map.hpp"
#include "log/Logger.h"

// Register factory: SelfOrganizingMap
static FlexoProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](FlexoProject& project) -> SharedPtr<SelfOrganizingMap> { return std::make_shared<SelfOrganizingMap>(project); }
};

SelfOrganizingMap& SelfOrganizingMap::Get(FlexoProject& project)
{
    return project.AttachedObjects::Get<SelfOrganizingMap>(factoryKey);
}

SelfOrganizingMap const& SelfOrganizingMap::Get(FlexoProject const& project)
{
    return Get(const_cast<FlexoProject&>(project));
}

SelfOrganizingMap::SelfOrganizingMap(FlexoProject& project)
    : m_isDone(false)
    , m_isTraining(false)
    , m_worker()
    , m_mut()
    , m_cv()
    , m_project(project)
{
    auto const& settings = ProjectSettings::Get(m_project);

    m_tmax = settings.GetMaxIterations();
    m_t = 0;

    m_project.Bind(EVT_PROJECT_SETTINGS_CHANGED, &SelfOrganizingMap::OnProjectSettingsChanged, this);
}

SelfOrganizingMap::~SelfOrganizingMap()
{
    StopWorker();
}

void SelfOrganizingMap::ToggleTraining()
{
    m_isTraining = !m_isTraining;
    m_cv.notify_one();

    if (m_isTraining) {
        log_info("SOM training resumed");
    } else {
        log_info("SOM training paused");
    }
}

bool SelfOrganizingMap::IsDone() const
{
    return m_isDone;
}

bool SelfOrganizingMap::IsTraining() const
{
    return m_isTraining;
}

void SelfOrganizingMap::OnProjectSettingsChanged(wxCommandEvent&)
{
    auto const& settings = ProjectSettings::Get(m_project);
    SetMaxIterations(settings.GetMaxIterations());
    SetLearningRate(settings.GetLearningRate());
    SetInitialNeighborhood(settings.GetNeighborhood());
}

void SelfOrganizingMap::SetMaxIterations(int numIterations)
{
    m_tmax = numIterations;
}

void SelfOrganizingMap::SetLearningRate(float rate)
{
    m_learnRate = LearningRate(rate, m_tmax);
}

void SelfOrganizingMap::SetInitialNeighborhood(float radius)
{
    m_neighborhood = Neighborhood(NeighborhoodRadius(radius, m_tmax));
}

int SelfOrganizingMap::GetIterations() const
{
    return m_t;
}

float SelfOrganizingMap::GetNeighborhood() const
{
    return m_neighborhood.radius(m_t);
}

float SelfOrganizingMap::GetInitialNeighborhood() const
{
    return m_neighborhood.radius.init;
}

float SelfOrganizingMap::GetLearningRate() const
{
    return m_learnRate(m_t);
}

void SelfOrganizingMap::StopWorker()
{
    m_isDone = true;
    m_cv.notify_one();

    if (m_worker.joinable()) {
        m_worker.join();
        log_info("The SOM worker joined successfully");
    }
}
