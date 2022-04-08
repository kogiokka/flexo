#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

typedef enum {
  OBJ_V = 0b0001,
  OBJ_VT = 0b0010,
  OBJ_VN = 0b0100,
} OBJ_Attr;

class ObjModel
{
  using Vec = std::vector<float>;
  using Triplet = std::array<int, 3>;
  using Face = std::vector<Triplet>;

  std::size_t drawArraysCount_;
  std::size_t stride_;
  std::vector<float> vertexBuffer_;
  std::vector<Vec> v_;
  std::vector<Vec> vt_;
  std::vector<Vec> vn_;
  std::vector<Face> f_;

public:
  ObjModel();
  ~ObjModel();
  void Read(std::string const& path);
  void GenVertexBuffer(std::uint16_t flag);
  std::size_t DrawArraysCount() const;
  std::size_t Stride() const;
  std::vector<Vec> const& V() const;
  std::vector<Vec> const& Vt() const;
  std::vector<Vec> const& Vn() const;
  std::vector<float> const& VertexBuffer() const;
};