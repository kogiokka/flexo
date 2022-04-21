#ifndef STL_IMPORTER_H
#define STL_IMPORTER_H

#include "Vertex.hpp"
#include "assetlib/BaseImporter.hpp"
#include "assetlib/ModelStructs.hpp"

#include <cstddef>
#include <cstdlib>
#include <string>
#include <vector>

class STLImporter : public BaseImporter
{
#pragma pack(push, 1) // Suppress padding
    struct Triangle {
        Vertex::Normal n;
        Vertex::Position p1;
        Vertex::Position p2;
        Vertex::Position p3;
        uint16_t attribByteCount;
    };
    static_assert(sizeof(Triangle) == 50); // In Binary STL, each triangle occupies 50 bytes.
#pragma pack(pop)

    STLModel model_;

public:
    virtual void Read(std::string const& filename) override;
    STLModel const& Model() const;

private:
    bool IsAsciiSTL() const;
    void ImportAsciiSTL();
    void ImportBinarySTL();
};

#endif
