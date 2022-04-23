#include "Mesh.hpp"

#include <cstddef>
#include <unordered_map>

using namespace std;

Mesh uvsphere;
Mesh surface;

Mesh::Mesh() { }

void Mesh::Import(STLModel const& model)
{
    vertices_.clear();

    Vertex v;
    for (size_t i = 0; i < model.normals.size(); i++) {
        v.normal = model.normals[i];
        for (size_t j = i * 3; j < (i + 1) * 3; j++) {
            v.position = model.positions[j];
            vertices_.push_back(v);
        }
    }
}

void Mesh::Import(OBJModel const& model)
{
    vertices_.clear();

    auto const& [positions, normals, faces] = model;
    unordered_map<size_t, vector<size_t>> faceIdxMap;

    for (OBJModel::Face const& face : faces) {
        auto const vertNum = face.size();
        // Convex polygon triangulation
        if (faceIdxMap.find(vertNum) == faceIdxMap.end()) {
            vector<size_t> idx;
            idx.reserve(3 * (vertNum - 2));
            for (size_t j = 1; j <= vertNum - 2; ++j) {
                idx.push_back(0);
                idx.push_back(j);
                idx.push_back(j + 1);
            }
            faceIdxMap.insert({ vertNum, idx });
        }

        Vertex vert;
        for (size_t i : faceIdxMap[vertNum]) {
            OBJModel::VertexRef const& vref = face[i];
            vert.position = positions[vref.v - 1];
            vert.normal = normals[vref.vn - 1];
            vertices_.push_back(vert);
        }
    }
}

std::vector<Vertex> const& Mesh::Vertices() const
{
    return vertices_;
}
