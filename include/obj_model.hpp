#pragma once

#include "util.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

typedef enum {
  OBJ_V = 0b0001,
  OBJ_VT = 0b0010,
  OBJ_VN = 0b0100,
} OBJ_Attr;

class OBJModel
{
  using Vec3 = std::array<float, 3>;
  using Triplet = std::array<int, 3>;
  using Face = std::vector<Triplet>;

  std::size_t drawArraysCount_;
  std::size_t stride_;
  std::vector<float> vertexBuffer_;
  std::vector<Vec3> v_;
  std::vector<Vec3> vt_;
  std::vector<Vec3> vn_;
  std::vector<Face> f_;

public:
  OBJModel();
  ~OBJModel();
  bool read(std::filesystem::path const& path);
  void genVertexBuffer(std::uint16_t flag);
  std::size_t drawArraysCount() const;
  std::size_t stride() const;
  std::vector<Vec3> const& v() const;
  std::vector<Vec3> const& vt() const;
  std::vector<Vec3> const& vn() const;
  std::vector<float> const& vertexBuffer() const;
};
