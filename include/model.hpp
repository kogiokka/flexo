#pragma once

#include "util.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

class Model
{
  std::vector<std::array<float, 3>> m_positions;
  std::vector<std::array<float, 3>> m_normals;
  std::vector<std::uint16_t> m_posIdx;
  std::vector<std::uint16_t> m_normIdx;

public:
  Model();
  bool readOBJ(std::string path);
  std::vector<std::array<float, 3>> const& positions() const;
  std::vector<float> vertexBuffer() const;
  std::size_t vertexCount() const;
  std::size_t vertexRenderSize() const;
};
