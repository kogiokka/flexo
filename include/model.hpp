#pragma once

#include "util.hpp"

#include <array>
#include <fstream>
#include <string>
#include <vector>

class Model
{
  int m_vertexCount;
  std::vector<float> m_vertexBuffer;

public:
  Model();
  bool readOBJ(std::string path);
  std::vector<float> const& vertexBuffer() const;
  int vertexCount() const;
};
