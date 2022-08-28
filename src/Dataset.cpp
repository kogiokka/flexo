#include "Dataset.hpp"
#include "Project.hpp"
#include "RandomRealNumber.hpp"

Dataset::Dataset()
    : m_pos()
    , m_rng()
{
}

Dataset::Dataset(std::vector<glm::vec3> const& positions)
    : m_pos(positions)
{
    m_rng.setRange(0, m_pos.size() - 1);
}

glm::vec3 const& Dataset::GetInput()
{
    return m_pos[m_rng.scalar()];
}

// Register factory: Dataset
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<DatasetList> { return std::make_shared<DatasetList>(project); }
};

DatasetList& DatasetList::Get(WatermarkingProject& project)
{
    return project.AttachedObjects::Get<DatasetList>(factoryKey);
}

DatasetList const& DatasetList::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

DatasetList::DatasetList(WatermarkingProject& project)
    : m_project(project)
{
}

void DatasetList::Add(std::vector<glm::vec3> const& positions)
{
    emplace_back(std::make_shared<Dataset>(positions));
}

void DatasetList::SetCurrent(unsigned int index)
{
    if (index >= size()) {
        return;
    }
    m_curr = operator[](index);
}

std::shared_ptr<Dataset> DatasetList::GetCurrent() const
{
    return m_curr;
}
