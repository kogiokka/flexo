#pragma once

#include <array>
#include <cstddef>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

class VolumeData
{
  enum class DataType { UnsignedChar, UnsignedShort, Float };
  std::unordered_map<std::string, std::string> info_;
  std::array<int, 3> resolution_;
  std::array<float, 3> ratio_;
  std::vector<float> scalarField_;
  std::vector<float> textureData_;
  DataType dataType_;

public:
  VolumeData(std::string const& infFile);
  ~VolumeData();
  VolumeData(VolumeData const&) = delete;
  VolumeData& operator=(VolumeData const&) = delete;

  void SetRawFile(std::string const& rawFile);
  bool Read();
  bool Write(std::string const& dirPath, std::string const& name) const;
  void GenTextureData();
  std::array<int, 3> Resolution() const;
  std::array<float, 3> Ratio() const;
  std::array<float, 3> Dimensions() const;
  std::array<float, 3> Position(std::size_t index);
  float DifferenceX(int x, int y, int z) const;
  float DifferenceY(int x, int y, int z) const;
  float DifferenceZ(int x, int y, int z) const;
  float const* TextureData() const;
  std::string InfFile() const;
  std::string RawFile() const;
  std::vector<float>& Data();
  std::vector<float> const& Data() const;
  float Min() const;
  float Max() const;
  bool Loaded() const;
  std::size_t Index(int x, int y, int z) const;
  float FunctionValue(int x, int y, int z) const;
  float FunctionValue(std::size_t index) const;
};

inline std::size_t
VolumeData::Index(int x, int y, int z) const
{
  auto const rezX = resolution_[0];
  auto const rezY = resolution_[1];

  return static_cast<std::size_t>(x + y * rezX + z * rezY * rezX);
}

inline float
VolumeData::FunctionValue(int x, int y, int z) const
{
  return scalarField_[Index(x, y, z)];
}

inline float
VolumeData::FunctionValue(std::size_t index) const
{
  return scalarField_[index];
}
