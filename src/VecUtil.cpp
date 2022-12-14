#include "VecUtil.hpp"

void SetVec3(Vec<2, float>& to, glm::vec2 const& from)
{
    to.x = from.x;
    to.y = from.y;
}

void SetVec3(Vec<3, float>& to, glm::vec3 const& from)
{
    to.x = from.x;
    to.y = from.y;
    to.z = from.z;
}

void SetVec2(glm::vec2& to, Vec<2, float> const& from)
{
    to.x = from.x;
    to.y = from.y;
}

void SetVec2(glm::vec3& to, Vec<3, float> const& from)
{
    to.x = from.x;
    to.y = from.y;
    to.z = from.z;
}
