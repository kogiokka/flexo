#include "ObjModel.hpp"

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <utility>

ObjModel::ObjModel()
  : drawArraysCount_(0)
  , stride_(0)
  , vertexBuffer_(0)
  , v_(0)
  , vt_(0)
  , vn_(0)
  , f_(0)
{
}

ObjModel::~ObjModel() {}

void
ObjModel::read(std::string const& path)
{
  using namespace std;

  ifstream file(path);
  if (file.fail()) {
    cerr << "Failed to open OBJ file: " << path << endl;
    exit(EXIT_FAILURE);
  }

  string line;
  while (getline(file, line)) {
    using namespace util::str;

    if (startsWith(line, "v ")) {

      auto tokens = split(line, R"(\s+)");
      tokens.erase(tokens.begin());
      auto const dimen = tokens.size();
      assert(dimen > 0 && dimen <= 4);

      std::vector<float> v;
      v.reserve(dimen);
      for (auto t : tokens) {
        v.push_back(stof(t));
      }
      v_.push_back(v);

    } else if (startsWith(line, "vt ")) {

      auto tokens = split(line, R"(\s+)");
      tokens.erase(tokens.begin());
      auto const dimen = tokens.size();

      assert(dimen > 0 && dimen <= 3);

      std::vector<float> vt;
      vt.reserve(dimen);
      for (auto t : tokens) {
        vt.push_back(stof(t));
      }
      vt_.push_back(vt);

    } else if (startsWith(line, "vn ")) {

      auto tokens = split(line, R"(\s+)");
      tokens.erase(tokens.begin());
      auto const dimen = tokens.size();

      assert(dimen > 0 && dimen <= 3);

      std::vector<float> vn;
      vn.reserve(dimen);
      for (auto t : tokens) {
        vn.push_back(stof(t));
      }
      vn_.push_back(vn);

    } else if (startsWith(line, "f ")) { // Grouping

      auto tokens = split(line, R"(\s+)");
      tokens.erase(tokens.begin());
      auto const dimen = tokens.size();

      assert(dimen >= 3);

      Face face;
      face.reserve(dimen);
      for (auto const& t : tokens) {
        auto const refNums = split(t, R"(/)");

        assert(refNums.size() <= 3);

        Triplet tri;
        for (int i = 0; i < 3; ++i) {
          if (refNums[i].empty()) {
            tri[i] = -1;
          } else {
            tri[i] = stoi(refNums[i]);
          }
        }

        face.push_back(tri);
      }
      f_.push_back(face);
    }
  }
}

void
ObjModel::genVertexBuffer(std::uint16_t flag)
{
  using namespace std;

  vector<float> buf;
  size_t stride = 0;

  bool const flagV = (flag & OBJ_V);
  bool const flagVT = (flag & OBJ_VT);
  bool const flagVN = (flag & OBJ_VN);

  stride += flagV ? v_.front().size() : 0;
  stride += flagVT ? vt_.front().size() : 0;
  stride += flagVN ? vn_.front().size() : 0;

  buf.reserve(f_.size() * stride);

  unordered_map<size_t, vector<float>> faceIdxMap;

  for (auto const& face : f_) {
    auto const vertNum = face.size();
    // Convex polygon triangulation
    if (faceIdxMap.find(vertNum) == faceIdxMap.end()) {
      vector<float> idx;
      idx.reserve(3 * (vertNum - 2));
      for (int j = 1; j <= vertNum - 2; ++j) {
        idx.push_back(0);
        idx.push_back(j);
        idx.push_back(j + 1);
      }
      faceIdxMap.insert({vertNum, idx});
    }

    for (int i : faceIdxMap[vertNum]) {
      if (flagV) {
        auto const idx = face[i][0] - 1;
        assert(idx >= 0);
        buf.insert(buf.end(), v_[idx].begin(), v_[idx].end());
      }
      if (flagVT) {
        auto const idx = face[i][1] - 1;
        assert(idx >= 0);
        buf.insert(buf.end(), vt_[idx].begin(), vt_[idx].end());
      }
      if (flagVN) {
        auto const idx = face[i][2] - 1;
        assert(idx >= 0);
        buf.insert(buf.end(), vn_[idx].begin(), vn_[idx].end());
      }
      drawArraysCount_ += 3;
    }
  }

  stride_ = stride * sizeof(float);
  vertexBuffer_ = move(buf);
}

std::vector<float> const&
ObjModel::vertexBuffer() const
{
  assert(!vertexBuffer_.empty());
  return vertexBuffer_;
}

std::size_t
ObjModel::stride() const
{
  return stride_;
}

std::size_t
ObjModel::drawArraysCount() const
{
  return drawArraysCount_;
}

std::vector<std::vector<float>> const&
ObjModel::v() const
{
  return v_;
}

std::vector<std::vector<float>> const&
ObjModel::vt() const
{
  return vt_;
}

std::vector<std::vector<float>> const&
ObjModel::vn() const
{
  return vn_;
}
