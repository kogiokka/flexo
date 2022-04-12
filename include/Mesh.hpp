#ifndef MESH_H
#define MESH_H

#include "assetlib/ModelStructs.hpp"

class Mesh
{
  std::vector<Vertex> vertices_;

public:
  Mesh();
  void Import(OBJModel const& model);
  void Import(STLModel const& model);
  std::vector<Vertex> const& Vertices() const;
};

#endif
