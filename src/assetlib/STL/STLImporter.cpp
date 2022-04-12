#include "assetlib/STL/STLImporter.hpp"
#include "Util.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>

bool
STLImporter::IsAsciiSTL() const
{
  std::string_view solidStr(buffer_.data(), 5);
  return solidStr == "solid";
}

void
STLImporter::Read(std::string const& filename)
{
  Slurp(filename);

  if (IsAsciiSTL()) {
    ImportAsciiSTL();
  } else {
    ImportBinarySTL();
  }
}

void
STLImporter::ImportAsciiSTL()
{
  using namespace std;

  istringstream iss(std::string(buffer_.cbegin(), buffer_.cend()));

  vector<string> const keywords = {
    "solid",
    "facet",
    "normal",
    "outer",
    "loop",
    "vertex",
    "vertex",
    "vertex",
    "endloop",
    "endfacet",
    "endsolid",
  };

  Vertex::Position p;
  Vertex::Normal n;
  string token;
  int state = 0;
  while (iss >> token) {
    if (token == keywords.front()) {
      getline(iss, model_.name);
      state = 1;
      continue;
    }
    if (token == keywords.back()) {
      break;
    }
    if (token == keywords[state]) {
      switch (state) {
      case 1: {
        state = 2;
      } break;
      case 2: {
        bool const success = iss >> n.x && iss >> n.y && iss >> n.z;
        if (!success) {
          cerr << "Error: Wrong normal format" << endl;
          exit(EXIT_FAILURE);
        }
        model_.normals.push_back(n);
        state = 3;
      } break;
      case 3:
        state = 4;
        break;
      case 4:
        state = 5;
        break;
      case 5:
      case 6:
      case 7: {
        bool const success = iss >> p.x && iss >> p.y && iss >> p.z;
        if (!success) {
          cerr << "Error: Wrong vertex format" << endl;
          exit(EXIT_FAILURE);
        }
        model_.positions.push_back(p);
        state = state + 1;
      } break;
      case 8:
        state = 9;
        break;
      case 9: {
        state = 1;
      } break;
      default:
        break;
      }
    } else {
      cout << "Wrong format. Expecting the token to be \"" << keywords[state] << "\", got \"" << token << "\" instead."
           << endl;
      exit(EXIT_FAILURE);
    }
  }
}

void
STLImporter::ImportBinarySTL()
{
  throw NotImplementedException();
}

STLModel const&
STLImporter::Model() const
{
  return model_;
}
