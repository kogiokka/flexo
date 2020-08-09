#include "random_vec3.hpp"

RandomVec3::RandomVec3()
  : m_rd{}
  , m_engine{m_rd()}
  , m_range{0.0f, 1.0f}
{
}

std::array<float, 3>
RandomVec3::color()
{
  float const r = m_range(m_engine);
  float const g = m_range(m_engine);
  float const b = m_range(m_engine);

  return std::array<float, 3>{r, g, b};
}
