#ifndef BASE_IMPORTER_H
#define BASE_IMPORTER_H

#include <string>
#include <vector>

class BaseImporter
{
protected:
    std::vector<unsigned char> buffer_;
    void Slurp(std::string const& filename);

public:
    virtual void Read(std::string const& filename) = 0;
};

#endif
