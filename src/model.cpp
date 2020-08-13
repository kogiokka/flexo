#include "model.hpp"

Model::Model()
  : m_positions(0)
  , m_normals(0)
  , m_posIdx(0)
  , m_normIdx(0)
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
        m_posIdx.push_back(stoi(idx[0]) - 1);
        m_normIdx.push_back(stoi(idx[2]) - 1);
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

std::vector<float>
Model::vertexBuffer() const
{
  std::vector<float> buffer;

  for (auto i : m_posIdx) {
    auto const pos = m_positions[i];
    buffer.insert(buffer.end(), pos.begin(), pos.end());
  }
  for (auto i : m_normIdx) {
    auto const norm = m_normals[i];
    buffer.insert(buffer.end(), norm.begin(), norm.end());
  }

  return buffer;
}

std::size_t
Model::vertexCount() const
{
  return m_positions.size();
}

std::size_t
Model::drawArrayCount() const
{
  return m_posIdx.size();
}
