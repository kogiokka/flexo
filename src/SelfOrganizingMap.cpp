#include "SelfOrganizingMap.hpp"
#include "Lattice.hpp"
#include "Project.hpp"
#include "ProjectSettings.hpp"
#include "common/Logger.hpp"

// Register factory: SelfOrganizingMap
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<SelfOrganizingMap> {
        return std::make_shared<SelfOrganizingMap>(project);
    }
};

SelfOrganizingMap& SelfOrganizingMap::Get(WatermarkingProject& project)
{
    return project.AttachedObjects::Get<SelfOrganizingMap>(factoryKey);
}

SelfOrganizingMap const& SelfOrganizingMap::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

SelfOrganizingMap::SelfOrganizingMap(WatermarkingProject& project)
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
        Logger::info("Training resumed");
    } else {
        Logger::info("Training paused");
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
        Logger::info("The worker joined successfully");
    }
}
