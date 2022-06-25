#include "InputData.hpp"

InputData::InputData()
    : m_pos()
    , m_rng()
{
}

InputData::InputData(std::vector<glm::vec3> const& positions)
    : m_pos(positions)
{
    m_rng.setRange(0, m_pos.size() - 1);
}

glm::vec3 const& InputData::GetInput()
{
    return m_pos[m_rng.scalar()];
}
