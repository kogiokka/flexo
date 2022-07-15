#include "Project.hpp"

WatermarkingProject::WatermarkingProject()
    : m_conf {}
{
}

void WatermarkingProject::Train()
{
    int const width = m_conf.GetLatticeWidth();
    int const height = m_conf.GetLatticeHeight();
    m_lattice = std::make_shared<Lattice>(width, height);
    m_lattice->flags = m_conf.GetLatticeFlags();

    // float neighborhood = 0.5f * sqrt(width * width + height * height);
    m_som = std::make_shared<SelfOrganizingMap>(m_conf.GetLearningRate(), m_conf.GetMaxIterations(),
                                                m_conf.GetNeighborhood());
}

void WatermarkingProject::Stop()
{
    m_som = nullptr;
}

bool WatermarkingProject::IsTraining() const
{
    return (m_som != nullptr) && (m_som->IsTraining());
}

bool WatermarkingProject::IsDone() const
{
    return m_som->IsDone();
}

int WatermarkingProject::GetIterations() const
{
    return m_som->GetIterations();
}

float WatermarkingProject::GetNeighborhood() const
{
    return m_som->GetNeighborhood();
}
