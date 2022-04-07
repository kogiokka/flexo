#include "assetlib/STL/STLImporter.hpp"
#include "Util.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>

STLImporter::STLImporter()
  : buffer_{} {};

STLImporter::~STLImporter(){};

bool
STLImporter::IsAsciiSTL() const
{
  std::string_view solidStr(buffer_.data(), 5);
  return solidStr == "solid";
}

void
STLImporter::Read(std::string const& filename)
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
  buffer_.resize(fileSize);
  fh.read(buffer_.data(), fileSize);
  fh.close();

  if (IsAsciiSTL()) {
    ImportAsciiSTL();
  } else {
    ImportBinarySTL();
  }
}

void
STLImporter::ImportAsciiSTL()
{
  throw NotImplementedException();
}

void
STLImporter::ImportBinarySTL()
{
  throw NotImplementedException();
}
