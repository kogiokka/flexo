#include "obj_model.hpp"

#include <fstream>

OBJModel::OBJModel()
  : drawArraysCount_(0)
  , vertexBuffer_(0)
  , v_(0)
  , vt_(0)
  , vn_(0)
  , f_(0)
{
}

OBJModel::~OBJModel() {}

bool
OBJModel::read(std::filesystem::path const& path)
{
  using namespace std;

  ifstream file(path);
  if (file.fail())
    return false;

  string line;
  while (getline(file, line)) {
    using namespace util::str;

    if (startsWith(line, "v ")) { // Position
      auto const tokens = split(line, R"(\s+)");
      auto const dimen = tokens.size();
      if (dimen == 4) {
        v_.emplace_back(Vec4{stof(tokens[1]), stof(tokens[2]), stof(tokens[3]), 1});
      } else if (dimen == 5) {
        v_.emplace_back(Vec4{stof(tokens[1]), stof(tokens[2]), stof(tokens[3]), stof(tokens[4])});
      }
    } else if (startsWith(line, "vt ")) { // Position
      auto const tokens = split(line, R"(\s+)");
      auto const dimen = tokens.size();
      if (dimen == 3) {
        vt_.emplace_back(Vec3{stof(tokens[1]), stof(tokens[2]), 1});
      } else if (dimen == 4) {
        vt_.emplace_back(Vec3{stof(tokens[1]), stof(tokens[2]), stof(tokens[3])});
      }
    } else if (startsWith(line, "vn ")) { // Vector
      auto const tokens = split(line, R"(\s+)");
      auto const dimen = tokens.size();
      if (dimen == 3) {
        vn_.emplace_back(Vec3{stof(tokens[1]), stof(tokens[2]), 0});
      } else if (dimen == 4) {
        vn_.emplace_back(Vec3{stof(tokens[1]), stof(tokens[2]), stof(tokens[3])});
      }
    } else if (startsWith(line, "f ")) { // Grouping
      auto const tokens = split(line, R"(\s+)");
      if (tokens.size() > 4)
        return false;

      Face face;
      for (int t = 1; t < tokens.size(); ++t) {
        auto const refs = split(tokens[t], R"(/)");
        face.emplace_back(Triplet{stoi(refs[0]), stoi(refs[1]), stoi(refs[2])});
      }
      f_.push_back(face);
    }
  }

  file.close();
  return true;
}

void
OBJModel::genVertexBuffer(std::uint16_t flag)
{
  std::vector<float> buf;
  buf.reserve(f_.size() * 6);

  for (auto const& group : f_) {
    for (auto const& triplet : group) {
      if (flag & OBJ_V) {
        Vec4 const v = v_[triplet[0] - 1];
        buf.push_back(v[0]);
        buf.push_back(v[1]);
        buf.push_back(v[2]);
      }
      if (flag & OBJ_VT) {
        Vec3 const vt = vt_[triplet[1] - 1];
        buf.push_back(vt[0]);
        buf.push_back(vt[1]);
        buf.push_back(vt[2]);
      }
      if (flag & OBJ_VN) {
        Vec3 const vn = vn_[triplet[2] - 1];
        buf.push_back(vn[0]);
        buf.push_back(vn[1]);
        buf.push_back(vn[2]);
      }
    }
    drawArraysCount_ += group.size();
  }

  vertexBuffer_ = std::move(buf);
}

std::vector<float> const&
OBJModel::vertexBuffer() const
{
  assert(!vertexBuffer_.empty());
  return vertexBuffer_;
}

std::size_t
OBJModel::drawArraysCount() const
{
  return drawArraysCount_;
}

std::vector<std::array<float, 4>> const&
OBJModel::v() const
{
  return v_;
}

std::vector<std::array<float, 3>> const&
OBJModel::vt() const
{
  return vt_;
}

std::vector<std::array<float, 3>> const&
OBJModel::vn() const
{
  return vn_;
}
