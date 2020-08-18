#include "model.hpp"

Model::Model()
  : m_positions(0)
  , m_normals(0)
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

  string line;
  while (getline(file, line)) {
    if (util::str::startsWith(line, "v ")) {
      vector<string> const tokens = util::str::split(line, R"(\s+)");
      m_positions.push_back(array<float, 3>{stof(tokens[1]), stof(tokens[2]), stof(tokens[3])});
    } else if (util::str::startsWith(line, "vn ")) {
      vector<string> const tokens = util::str::split(line, R"(\s+)");
      m_normals.push_back(array<float, 3>{stof(tokens[1]), stof(tokens[2]), stof(tokens[3])});
    } else if (util::str::startsWith(line, "f ")) {
      vector<string> const group = util::str::split(line, R"(\s+)");
      if (group.size() > 4)
        return false;
      for (int v = 1; v < group.size(); ++v) {
        vector<string> const idx = util::str::split(group[v], R"(/)");
        auto const pos = m_positions[stoi(idx[0]) - 1];
        auto const norm = m_normals[stoi(idx[2]) - 1];
        m_vertexBuffer.insert(m_vertexBuffer.end(), pos.begin(), pos.end());
        m_vertexBuffer.insert(m_vertexBuffer.end(), norm.begin(), norm.end());
        m_drawArrayCount += 3;
      }
    }
  }
  file.close();

  return true;
}

std::vector<std::array<float, 3>> const&
Model::positions() const
{
  return m_positions;
}

std::vector<std::array<float, 3>> const&
Model::normals() const
{
  return m_normals;
}

std::vector<float> const&
Model::vertexBuffer() const
{
  return m_vertexBuffer;
}

std::size_t
Model::drawArrayCount() const
{
  return m_drawArrayCount;
}
