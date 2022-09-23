#include "Dataset.hpp"

template <int InDim>
Dataset<InDim>::Dataset()
    : m_pos()
    , m_rng()
{
}

template <int InDim>
Dataset<InDim>::Dataset(std::vector<glm::vec3> const& positions)
    : m_pos()
    , m_rng()
{
    std::vector<Vec<InDim>> data;
    data.reserve(positions.size());
    for (auto const& p : positions) {
        data.emplace_back(p.x, p.y, p.z);
    }

    m_pos = data;

    m_rng.setRange(0, m_pos.size() - 1);
    CalculateBoundingBox();
}

template <int InDim>
Dataset<InDim>::Dataset(std::vector<Vec<InDim>> const& positions)
    : m_pos(positions)
    , m_rng()
{
    m_rng.setRange(0, m_pos.size() - 1);
    CalculateBoundingBox();
}

template <int InDim>
void Dataset<InDim>::Insert(std::vector<glm::vec3> const& positions)
{
    std::vector<Vec<InDim>> data;
    data.reserve(positions.size());
    for (auto const& p : positions) {
        data.emplace_back(p.x, p.y, p.z);
    }

    m_pos.insert(m_pos.end(), data.begin(), data.end());
    m_rng.setRange(0, m_pos.size() - 1);
    CalculateBoundingBox();
}

template <int InDim>
void Dataset<InDim>::Insert(std::vector<Vec<InDim>> const& positions)
{
    m_pos.insert(m_pos.end(), positions.begin(), positions.end());
    m_rng.setRange(0, m_pos.size() - 1);
    CalculateBoundingBox();
}

template <int InDim>
std::vector<Vec<InDim>> const& Dataset<InDim>::GetData() const
{
    return m_pos;
}

template <int InDim>
Vec<InDim> const& Dataset<InDim>::GetInput()
{
    return m_pos[m_rng.scalar()];
}

template <int InDim>
BoundingBox const& Dataset<InDim>::GetBoundingBox() const
{
    return m_box;
}

template <int InDim>
void Dataset<InDim>::CalculateBoundingBox()
{
    const float FLOAT_MAX = std::numeric_limits<float>::max();
    glm::vec3 min = { FLOAT_MAX, FLOAT_MAX, FLOAT_MAX };
    glm::vec3 max = { -FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX };

    for (auto const& p : m_pos) {
        if (p[0] > max.x) {
            max.x = p[0];
        }
        if (p[1] > max.y) {
            max.y = p[1];
        }
        if (p[2] > max.z) {
            max.z = p[2];
        }
        if (p[0] < min.x) {
            min.x = p[0];
        }
        if (p[1] < min.y) {
            min.y = p[1];
        }
        if (p[2] < min.z) {
            min.z = p[2];
        }
    }

    m_box = { max, min };
}
