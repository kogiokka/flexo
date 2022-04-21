#include "assetlib/OBJ/OBJImporter.hpp"
#include "assetlib/ModelStructs.hpp"

#include <iostream>
#include <sstream>

void OBJImporter::Read(std::string const& filename)
{
    Slurp(filename);

    using namespace std;

    istringstream iss(string(buffer_.cbegin(), buffer_.cend()));

    string token;
    Vertex::Position p;
    Vertex::Normal n;

    auto& [v, vn, f] = model_;
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
}

OBJModel const& OBJImporter::Model() const
{
    return model_;
}
