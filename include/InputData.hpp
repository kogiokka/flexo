#ifndef INCLUDE_DATA_H
#define INCLUDE_DATA_H

#include <glm/glm.hpp>

#include "RandomIntNumber.hpp"

class InputData
{
    std::vector<glm::vec3> m_pos;
    RandomIntNumber<unsigned int> m_rng;

public:
    InputData();
    InputData(std::vector<glm::vec3> const& positions);
    glm::vec3 const& GetInput();
};

#endif
