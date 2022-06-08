#ifndef STL_IMPORTER_H
#define STL_IMPORTER_H

#include <cstdlib>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "assetlib/BaseImporter.hpp"

class STLImporter : public BaseImporter
{
#pragma pack(push, 1) // Suppress padding
    struct Triangle {
        glm::vec3 n;
        glm::vec3 p1;
        glm::vec3 p2;
        glm::vec3 p3;
        uint16_t attribByteCount;
    };
    static_assert(sizeof(Triangle) == 50); // In Binary STL, each triangle occupies 50 bytes.
#pragma pack(pop)

public:
    [[nodiscard]] virtual Mesh ReadFile(std::string const& filename) override;

private:
    bool IsAsciiSTL() const;
    Mesh ImportAsciiSTL();
    Mesh ImportBinarySTL();
};

#endif
