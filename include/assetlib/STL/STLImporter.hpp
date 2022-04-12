#ifndef STL_IMPORTER_H
#define STL_IMPORTER_H

#include "Vertex.hpp"
#include "assetlib/BaseImporter.hpp"

#include <string>
#include <vector>

class STLImporter : public BaseImporter
{
  std::string name_;
  std::vector<Vertex> vertices_;

public:
  STLImporter();
  ~STLImporter();
  virtual void Read(std::string const& filename) override;
  std::vector<Vertex> const& Vertices() const;

private:
  bool IsAsciiSTL() const;
  void ImportAsciiSTL();
  void ImportBinarySTL();
};

#endif
