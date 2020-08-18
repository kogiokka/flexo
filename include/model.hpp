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
  unsigned int m_drawArrayCount;
  std::vector<float> m_vertexBuffer;
  std::vector<std::array<float, 3>> m_positions;
  std::vector<std::array<float, 3>> m_normals;

public:
  Model();
  bool readOBJ(std::string path);
  std::vector<float> const& vertexBuffer() const;
  std::vector<std::array<float, 3>> const& positions() const;
  std::vector<std::array<float, 3>> const& normals() const;
  std::size_t drawArrayCount() const;
};
