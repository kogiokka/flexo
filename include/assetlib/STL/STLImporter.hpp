#ifndef STL_IMPORTER_H
#define STL_IMPORTER_H

#include "Vertex.hpp"

#include <string>
#include <vector>

enum State : int {
  SOLID = 0,
  FACET = 1,
  NORMAL = 1 << 1,
  OUTER = 1 << 2,
  LOOP = 1 << 3,
  VERTEX = 1 << 4,
  ENDLOOP = 1 << 5,
  ENDFACET = 1 << 6,
  ENDSOLID = 1 << 7,
  READ_NORMAL,
  READ_VERTEX,
};

class STLImporter
{
  std::string name_;
  std::vector<char> buffer_;
  std::vector<Vertex> vertices_;

public:
  STLImporter();
  ~STLImporter();
  void Read(std::string const& filename);
  std::vector<Vertex> const& Vertices() const;

private:
  bool IsAsciiSTL() const;
  void ImportAsciiSTL();
  void ImportBinarySTL();
};

#endif
