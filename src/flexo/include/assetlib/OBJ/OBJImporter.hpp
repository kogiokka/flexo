#ifndef OBJ_IMPORTER_H
#define OBJ_IMPORTER_H

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "assetlib/BaseImporter.hpp"

class OBJImporter : public BaseImporter
{
public:
    [[nodiscard]] virtual Mesh ReadFile(std::string const& filename) override;

private:
    struct OBJModel {
        struct VertexRef {
            int v, vt, vn;
        };

        using Face = std::vector<VertexRef>;

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<Face> faces;
    };

    Mesh BuildMesh(OBJModel const& model) const;
};

#endif
