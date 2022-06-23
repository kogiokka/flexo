#ifndef INCLUDE_DATA_H
#define INCLUDE_DATA_H


#include <glm/glm.hpp>

#include "RandomIntNumber.hpp"

class InputData
{
    std::vector<glm::vec3> pos_;
    RandomIntNumber<unsigned int> rng_;

public:
    InputData();
    InputData(std::vector<glm::vec3> const& positions);
    glm::vec3 const& GetInput();
};

#endif
