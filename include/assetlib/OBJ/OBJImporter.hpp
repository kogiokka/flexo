#ifndef OBJ_IMPORTER_H
#define OBJ_IMPORTER_H

#include "Vertex.hpp"
#include "assetlib/BaseImporter.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class OBJImporter : public BaseImporter
{
  struct VertexRef {
    int v, vt, vn;
  };

  using Vec = std::vector<float>;
  using Face = std::vector<VertexRef>;

  std::vector<Vertex::Position> v_;
  std::vector<Vertex::Normal> vn_;
  std::vector<Face> f_;

private:
public:
  OBJImporter();
  ~OBJImporter();
  virtual void Read(std::string const& filename) override;
  std::vector<Vertex> GenVertexBuffer() const;
};

#endif
