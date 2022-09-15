#ifndef INCLUDE_DATA_H
#define INCLUDE_DATA_H

#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "RandomIntNumber.hpp"

class InputData
{
public:
    InputData();
    InputData(std::vector<glm::vec3> const& positions);
    void Insert(std::vector<glm::vec3> const& positions);
    std::vector<glm::vec3> const& GetData() const;
    glm::vec3 const& GetInput();
    BoundingBox const& GetBoundingBox() const;

private:
    std::vector<glm::vec3> m_pos;
    RandomIntNumber<unsigned int> m_rng;
    BoundingBox m_box;

    void CalculateBoundingBox();
};

#endif
