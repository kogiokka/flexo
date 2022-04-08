#ifndef STL_IMPORTER_H
#define STL_IMPORTER_H

#include "Vertex.hpp"

#include <string>
#include <vector>

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
