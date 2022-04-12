#ifndef MODEL_STRUCT_H
#define MODEL_STRUCT_H

#include "Vertex.hpp"

#include <string>
#include <vector>

struct OBJModel {
  struct VertexRef {
    int v, vt, vn;
  };

  using Face = std::vector<VertexRef>;

  std::vector<Vertex::Position> positions;
  std::vector<Vertex::Normal> normals;
  std::vector<Face> faces;
};

struct STLModel {
  std::string name;
  std::vector<Vertex::Position> positions;
  std::vector<Vertex::Normal> normals;
};

#endif
