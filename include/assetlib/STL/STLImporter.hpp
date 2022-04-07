#ifndef STL_IMPORTER_H
#define STL_IMPORTER_H

#include <string>
#include <vector>

class STLImporter
{
  std::vector<char> buffer_;
public:
  STLImporter();
  ~STLImporter();
  void Read(std::string const& filename);

private:
  bool IsAsciiSTL() const;
  void ImportAsciiSTL();
  void ImportBinarySTL();
};

#endif
