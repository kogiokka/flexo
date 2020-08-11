#include "model.hpp"

Model::Model()
  : m_vertexCount(0)
  , m_vertexBuffer(0)
{
}

bool
Model::readOBJ(std::string path)
{
  using namespace std;

  ifstream file;
  file.open(path);

  if (file.fail())
    return false;

  vector<array<float, 3>> vert;
  vector<array<float, 3>> norm;
  vector<unsigned short> vertIdx;
  vector<unsigned short> normIdx;

  string line;
  while (getline(file, line)) {
    if (util::stringStartsWith(line, "v ")) {
      vector<string> const tokens = util::splitString(line, R"(\s+)");
      vert.push_back(array<float, 3>{stof(tokens[1]), stof(tokens[2]), stof(tokens[3])});
    } else if (util::stringStartsWith(line, "vn ")) {
      vector<string> const tokens = util::splitString(line, R"(\s+)");
      norm.push_back(array<float, 3>{stof(tokens[1]), stof(tokens[2]), stof(tokens[3])});
    } else if (util::stringStartsWith(line, "f ")) {
      vector<string> const group = util::splitString(line, R"(\s+)");
      if (group.size() > 4)
        return false;
      for (int v = 1; v < group.size(); ++v) {
        vector<string> const idx = util::splitString(group[v], R"(/)");
        auto const vertex = vert[stoi(idx[0]) - 1];
        auto const normal = norm[stoi(idx[2]) - 1];
        m_vertexBuffer.insert(m_vertexBuffer.end(), vertex.begin(), vertex.end());
        m_vertexBuffer.insert(m_vertexBuffer.end(), normal.begin(), normal.end());
        ++m_vertexCount;
      }
    }
  }
  file.close();

  return true;
}

std::vector<float> const&
Model::vertexBuffer() const
{
  return m_vertexBuffer;
}

int
Model::vertexCount() const
{
  return m_vertexCount;
}
