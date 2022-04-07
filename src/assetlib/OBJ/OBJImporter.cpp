#include "assetlib/OBJ/OBJImporter.hpp"

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
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
ObjModel::Read(std::string const& path)
{
  using namespace std;

  ifstream file(path);
  if (file.fail()) {
    cerr << "Failed to open OBJ file: " << path << endl;
    exit(EXIT_FAILURE);
  }

  string line;
  while (getline(file, line)) {

    istringstream lineStream(line);
    string type;
    lineStream >> type;

    if (type == "v") {
      float value;
      std::vector<float> v;
      v.reserve(4);
      while (!lineStream.eof()) {
        lineStream >> value;
        v.push_back(value);
      }
      v_.push_back(move(v));
    } else if (type == "vt") {
      float value;
      std::vector<float> vt;
      vt.reserve(3);
      while (!lineStream.eof()) {
        lineStream >> value;
        vt.push_back(value);
      }
      vt_.push_back(move(vt));
    } else if (type == "vn") {
      float value;
      std::vector<float> vn;
      vn.reserve(3);
      while (!lineStream.eof()) {
        lineStream >> value;
        vn.push_back(value);
      }
      vn_.push_back(move(vn));
    } else if (type == "f") {
      Face face;
      face.reserve(4);
      string refTripletStr;

      while (!lineStream.eof()) {
        Triplet refTriplet;
        lineStream >> refTripletStr;
        istringstream refTripletStream(refTripletStr);
        string refStr;
        for (int i = 0; i < 3; ++i) {
          getline(refTripletStream, refStr, '/');
          if (refStr.empty()) {
            refTriplet[i] = -1;
          } else {
            refTriplet[i] = stoi(refStr);
          }
        }
        face.push_back(move(refTriplet));
      }

      f_.push_back(move(face));
    }
  }
}

void
ObjModel::GenVertexBuffer(std::uint16_t flag)
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

  unordered_map<size_t, vector<std::size_t>> faceIdxMap;

  for (auto const& face : f_) {
    auto const vertNum = face.size();
    // Convex polygon triangulation
    if (faceIdxMap.find(vertNum) == faceIdxMap.end()) {
      vector<std::size_t> idx;
      idx.reserve(3 * (vertNum - 2));
      for (std::size_t j = 1; j <= vertNum - 2; ++j) {
        idx.push_back(0);
        idx.push_back(j);
        idx.push_back(j + 1);
      }
      faceIdxMap.insert({vertNum, idx});
    }

    for (std::size_t i : faceIdxMap[vertNum]) {
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
ObjModel::VertexBuffer() const
{
  assert(!vertexBuffer_.empty());
  return vertexBuffer_;
}

std::size_t
ObjModel::Stride() const
{
  return stride_;
}

std::size_t
ObjModel::DrawArraysCount() const
{
  return drawArraysCount_;
}

std::vector<std::vector<float>> const&
ObjModel::V() const
{
  return v_;
}

std::vector<std::vector<float>> const&
ObjModel::Vt() const
{
  return vt_;
}

std::vector<std::vector<float>> const&
ObjModel::Vn() const
{
  return vn_;
}
