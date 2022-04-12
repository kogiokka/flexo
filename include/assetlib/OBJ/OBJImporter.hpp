#ifndef OBJ_IMPORTER_H
#define OBJ_IMPORTER_H

#include "Vertex.hpp"
#include "assetlib/BaseImporter.hpp"
#include "assetlib/ModelStructs.hpp"

#include <string>
#include <vector>

class OBJImporter : public BaseImporter
{
  OBJModel model_;

public:
  virtual void Read(std::string const& filename) override;
  OBJModel const& Model() const;
};

#endif
