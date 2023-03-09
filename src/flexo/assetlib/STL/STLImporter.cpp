#include "assetlib/STL/STLImporter.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>

bool STLImporter::IsAsciiSTL() const
{
    std::string_view solidStr(reinterpret_cast<const char*>(m_buffer.data()), 5);
    return solidStr == "solid";
}

Mesh STLImporter::ReadFile(std::string const& filename)
{
    Slurp(filename);

    if (IsAsciiSTL()) {
        return ImportAsciiSTL();
    } else {
        return ImportBinarySTL();
    }
}

Mesh STLImporter::ImportAsciiSTL()
{
    Mesh mesh;

    using namespace std;

    istringstream iss(std::string(m_buffer.cbegin(), m_buffer.cend()));

    vector<string> const keywords = {
        "solid", "facet", "normal", "outer", "loop", "vertex", "vertex", "vertex", "endloop", "endfacet", "endsolid",
    };

    glm::vec3 p, n;
    string token, modelName;
    int state = 0;
    while (iss >> token) {
        if (token == keywords.front()) {
            getline(iss, modelName);
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
                mesh.normals.push_back(n);
                mesh.normals.push_back(n);
                mesh.normals.push_back(n);
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
                mesh.positions.push_back(p);
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
            cout << "Wrong format. Expecting the token to be \"" << keywords[state] << "\", got \"" << token
                 << "\" instead." << endl;
            exit(EXIT_FAILURE);
        }
    }

    return mesh;
}

Mesh STLImporter::ImportBinarySTL()
{
    Mesh mesh;

    unsigned int* numTriangles = reinterpret_cast<unsigned int*>(m_buffer.data() + 80);
    Triangle* triangles = reinterpret_cast<Triangle*>(m_buffer.data() + 84);

    for (unsigned int i = 0; i < *numTriangles; i++) {
        mesh.normals.push_back(triangles[i].n);
        mesh.normals.push_back(triangles[i].n);
        mesh.normals.push_back(triangles[i].n);
        mesh.positions.push_back(triangles[i].p1);
        mesh.positions.push_back(triangles[i].p2);
        mesh.positions.push_back(triangles[i].p3);
    }

    return mesh;
}
