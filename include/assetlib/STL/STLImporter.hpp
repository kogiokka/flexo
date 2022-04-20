#ifndef STL_IMPORTER_H
#define STL_IMPORTER_H

#include "Vertex.hpp"
#include "assetlib/BaseImporter.hpp"
#include "assetlib/ModelStructs.hpp"

#include <cstdlib>
#include <string>
#include <vector>

class STLImporter : public BaseImporter
{
  STLModel model_;

public:
  virtual void Read(std::string const& filename) override;
  STLModel const& Model() const;

private:
  bool IsAsciiSTL() const;
  void ImportAsciiSTL();
  void ImportBinarySTL();
  template<typename T>
  T extractNumber(std::size_t pos);
};

#endif
