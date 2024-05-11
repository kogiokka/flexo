#ifndef BASE_IMPORTER_H
#define BASE_IMPORTER_H

#include <string>
#include <vector>

#include "gfx/Mesh.hpp"

class BaseImporter
{
protected:
    std::vector<unsigned char> m_buffer;
    void Slurp(std::string const& filename);

public:
    virtual Mesh ReadFile(std::string const& filename) = 0;
};

#endif
