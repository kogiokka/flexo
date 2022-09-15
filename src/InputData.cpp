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
    CalculateBoundingBox();
}

void InputData::Insert(std::vector<glm::vec3> const& positions)
{
    m_pos.insert(m_pos.end(), positions.begin(), positions.end());
    m_rng.setRange(0, m_pos.size() - 1);
    CalculateBoundingBox();
}

std::vector<glm::vec3> const& InputData::GetData() const
{
    return m_pos;
}

glm::vec3 const& InputData::GetInput()
{
    return m_pos[m_rng.scalar()];
}

BoundingBox const& InputData::GetBoundingBox() const
{
    return m_box;
}

void InputData::CalculateBoundingBox()
{
    const float FLOAT_MAX = std::numeric_limits<float>::max();
    glm::vec3 min = { FLOAT_MAX, FLOAT_MAX, FLOAT_MAX };
    glm::vec3 max = { -FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX };

    for (auto const& p : m_pos) {
        if (p.x > max.x) {
            max.x = p.x;
        }
        if (p.y > max.y) {
            max.y = p.y;
        }
        if (p.z > max.z) {
            max.z = p.z;
        }
        if (p.x < min.x) {
            min.x = p.x;
        }
        if (p.y < min.y) {
            min.y = p.y;
        }
        if (p.z < min.z) {
            min.z = p.z;
        }
    }

    m_box = { max, min };
}
