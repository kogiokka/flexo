#include "obj_model.hpp"

#include <fstream>

OBJModel::OBJModel()
  : drawArraysCount_(0)
  , stride_(0)
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
      assert(tokens.size() > 0 && tokens.size() <= 4);
      v_.emplace_back(Vec3{stof(tokens[1]), stof(tokens[2]), stof(tokens[3])});

    } else if (startsWith(line, "vt ")) { // Position

      auto const tokens = split(line, R"(\s+)");
      assert(tokens.size() > 0 && tokens.size() <= 4);
      auto const dimen = tokens.size() - 1;
      switch (dimen) {
      case 1:
        vt_.emplace_back(Vec3{stof(tokens[1]), 0, 0});
        break;
      case 2:
        vt_.emplace_back(Vec3{stof(tokens[1]), stof(tokens[2]), 0});
        break;
      case 3:
        vt_.emplace_back(Vec3{stof(tokens[1]), stof(tokens[2]), stof(tokens[3])});
        break;
      }

    } else if (startsWith(line, "vn ")) { // Vector

      auto const tokens = split(line, R"(\s+)");
      assert(tokens.size() > 0 && tokens.size() <= 4);
      vn_.emplace_back(Vec3{stof(tokens[1]), stof(tokens[2]), stof(tokens[3])});

    } else if (startsWith(line, "f ")) { // Grouping

      auto const tokens = split(line, R"(\s+)");
      assert(tokens.size() >= 3);
      Face face;
      for (int t = 1; t < tokens.size(); ++t) {
        auto const refs = split(tokens[t], R"(/)");
        assert(refs.size() <= 3);
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
  unsigned int stride = 0;
  std::vector<float> buf;

  bool const flagV = (flag & OBJ_V);
  bool const flagVT = (flag & OBJ_VT);
  bool const flagVN = (flag & OBJ_VN);

  stride += flagV ? 3 : 0;
  stride += flagVT ? 3 : 0;
  stride += flagVN ? 3 : 0;

  buf.reserve(f_.size() * stride);

  for (auto const& group : f_) {
    for (auto const& triplet : group) {
      if (flagV) {
        Vec3 const v = v_[triplet[0] - 1];
        buf.insert(buf.end(), v.begin(), v.end());
      }
      if (flagVT) {
        Vec3 const vt = vt_[triplet[1] - 1];
        buf.insert(buf.end(), vt.begin(), vt.end());
      }
      if (flagVN) {
        Vec3 const vn = vn_[triplet[2] - 1];
        buf.insert(buf.end(), vn.begin(), vn.end());
      }
    }
    drawArraysCount_ += group.size();
  }

  stride_ = stride * sizeof(float);
  vertexBuffer_ = std::move(buf);
}

std::vector<float> const&
OBJModel::vertexBuffer() const
{
  assert(!vertexBuffer_.empty());
  return vertexBuffer_;
}

std::size_t
OBJModel::stride() const
{
  return stride_;
}

std::size_t
OBJModel::drawArraysCount() const
{
  return drawArraysCount_;
}

std::vector<std::array<float, 3>> const&
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
