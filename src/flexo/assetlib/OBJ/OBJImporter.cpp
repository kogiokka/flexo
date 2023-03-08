#include "assetlib/OBJ/OBJImporter.hpp"

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

Mesh OBJImporter::ReadFile(std::string const& filename)
{
    using namespace std;

    OBJModel model;

    Slurp(filename);

    istringstream iss(string(m_buffer.cbegin(), m_buffer.cend()));

    string token;
    glm::vec3 p, n;

    auto& [v, vn, f] = model;
    while (iss >> token) {
        if (token == "v") {
            bool success = iss >> p.x && iss >> p.y && iss >> p.z;
            if (!success) {
                cerr << "Error occurred when parsing \"v\"." << endl;
                exit(EXIT_FAILURE);
            }
            v.push_back(p);
        } else if (token == "vt") {
            // TODO
        } else if (token == "vn") {
            bool success = iss >> n.x && iss >> n.y && iss >> n.z;
            if (!success) {
                cerr << "Error occurred when parsing \"vn\"." << endl;
                exit(EXIT_FAILURE);
            }
            vn.push_back(n);
        } else if (token == "f") {
            string line;
            string triref;
            getline(iss, line);
            istringstream lineiss(line);
            OBJModel::VertexRef vref;
            OBJModel::Face face;
            while (lineiss >> triref) {
                int ret = sscanf(triref.c_str(), "%d/%d/%d", &vref.v, &vref.vt, &vref.vn);
                if (ret != 3) {
                    ret = sscanf(triref.c_str(), "%d//%d", &vref.v, &vref.vn);
                    vref.vt = 0;
                    if (ret != 2) {
                        cerr << "Wront \"f\" format!" << endl;
                        exit(EXIT_FAILURE);
                    }
                }
                face.push_back(vref);
            }
            f.push_back(face);
        }
    }

    return BuildMesh(model);
}

Mesh OBJImporter::BuildMesh(OBJModel const& model) const
{
    using namespace std;

    Mesh mesh;

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

        for (size_t i : faceIdxMap[vertNum]) {
            OBJModel::VertexRef const& vref = face[i];
            mesh.positions.push_back(positions[vref.v - 1]);
            mesh.normals.push_back(normals[vref.vn - 1]);
        }
    }

    return mesh;
}
