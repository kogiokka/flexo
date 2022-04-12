#include "assetlib/STL/STLImporter.hpp"
#include "Util.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>

STLImporter::STLImporter()
  : BaseImporter()
  , name_("")
  , vertices_{} {};

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
  Slurp(filename);

  if (IsAsciiSTL()) {
    ImportAsciiSTL();
  } else {
    ImportBinarySTL();
  }
}

std::vector<Vertex> const&
STLImporter::Vertices() const
{
  return vertices_;
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

  Vertex v;
  string token;
  int state = 0;
  while (iss >> token) {
    if (token == keywords.front()) {
      getline(iss, name_);
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
        bool const success = iss >> v.normal.x && iss >> v.normal.y && iss >> v.normal.z;
        if (!success) {
          cerr << "Error: Wrong normal format" << endl;
          exit(EXIT_FAILURE);
        }
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
        bool const success = iss >> v.position.x && iss >> v.position.y && iss >> v.position.z;
        if (!success) {
          cerr << "Error: Wrong vertex format" << endl;
          exit(EXIT_FAILURE);
        }
        vertices_.push_back(v);
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
