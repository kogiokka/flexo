#include "assetlib/BaseImporter.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

void BaseImporter::Slurp(std::string const& filename)
{
    using namespace std;
    namespace fs = std::filesystem;

    ifstream fh;
    fh.open(filename, ios::binary);
    if (fh.fail()) {
        cerr << "Failed to open file: \"" << filename << "\"" << endl;
        exit(EXIT_FAILURE);
    }

    size_t fileSize = fs::file_size(filename);
    m_buffer.resize(fileSize);
    fh.read(reinterpret_cast<char*>(m_buffer.data()), fileSize);
    fh.close();
}
