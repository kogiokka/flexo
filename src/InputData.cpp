#include "InputData.hpp"

InputData::InputData()
    : pos_()
    , rng_()
{
}

InputData::InputData(std::vector<glm::vec3> const& positions)
    : pos_(positions)
{
    rng_.setRange(0, pos_.size() - 1);
}

glm::vec3 const& InputData::getInput()
{
    return pos_[rng_.scalar()];
}
