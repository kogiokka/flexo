#include "SelfOrganizingMap.hpp"
#include "Project.hpp"
#include "log/Logger.h"
#include "object/Map.hpp"

SelfOrganizingMap::~SelfOrganizingMap()
{
    m_isDone = true;
    m_cv.notify_one();

    if (m_worker.joinable()) {
        m_worker.join();
        log_info("The SOM worker joined successfully");
    }
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
