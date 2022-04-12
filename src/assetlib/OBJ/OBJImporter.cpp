#include "assetlib/OBJ/OBJImporter.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <utility>

OBJImporter::OBJImporter()
  : v_(0)
  , vn_(0)
  , f_(0)
{
}

OBJImporter::~OBJImporter() {}

void
OBJImporter::Read(std::string const& filename)
{
  Slurp(filename);

  using namespace std;

  vector<string> const keywords = {
    "v",
    "vt",
    "vn",
    "f",
  };

  istringstream iss(string(buffer_.cbegin(), buffer_.cend()));

  string token;
  Vertex::Position p;
  Vertex::Normal n;

  while (iss >> token) {
    if (token == "v") {
      bool success = iss >> p.x && iss >> p.y && iss >> p.z;
      if (!success) {
        cerr << "Error occurred when parsing \"v\"." << endl;
        exit(EXIT_FAILURE);
      }
      v_.push_back(p);
    } else if (token == "vt") {
      // TODO
    } else if (token == "vn") {
      bool success = iss >> n.x && iss >> n.y && iss >> n.z;
      if (!success) {
        cerr << "Error occurred when parsing \"vn\"." << endl;
        exit(EXIT_FAILURE);
      }
      vn_.push_back(n);
    } else if (token == "f") {
      string line;
      string triref;
      getline(iss, line);
      istringstream lineiss(line);
      VertexRef vref;
      Face face;
      while (lineiss >> triref) {
        int ret = sscanf(triref.c_str(), "%d/%d/%d", &vref.v, &vref.vt, &vref.vn);
        if (ret != 3) {
          cerr << "Wrong \"f\" format!" << endl;
          exit(EXIT_FAILURE);
        }
        face.push_back(vref);
      }
      f_.push_back(face);
    }
  }
}

std::vector<Vertex>
OBJImporter::GenVertexBuffer() const
{
  using namespace std;
  vector<Vertex> vbuf;

  unordered_map<size_t, vector<size_t>> faceIdxMap;

  for (Face const& face : f_) {
    auto const vertNum = face.size();
    // Convex polygon triangulation
    if (faceIdxMap.find(vertNum) == faceIdxMap.end()) {
      vector<size_t> idx;
      idx.reserve(3 * (vertNum - 2));
      for (size_t j = 1; j <= vertNum - 2; ++j) {
        idx.push_back(0);
        idx.push_back(j);
        idx.push_back(j + 1);
      }
      faceIdxMap.insert({vertNum, idx});
    }

    Vertex vert;
    for (size_t i : faceIdxMap[vertNum]) {
      VertexRef const& vref = face[i];
      vert.position = v_[vref.v - 1];
      vert.normal = vn_[vref.vn - 1];
      vbuf.push_back(vert);
    }
  }

  return vbuf;
}
