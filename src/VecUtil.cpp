#include "VecUtil.hpp"

void SetVec3(Vec<2, float>& to, glm::vec2 const& from)
{
    to[0] = from.x;
    to[1] = from.y;
}

void SetVec3(Vec<3, float>& to, glm::vec3 const& from)
{
    to[0] = from.x;
    to[1] = from.y;
    to[2] = from.z;
}

void SetVec2(glm::vec2& to, Vec<2, float> const& from)
{
    to[0] = from[0];
    to[1] = from[1];
}

void SetVec2(glm::vec3& to, Vec<3, float> const& from)
{
    to[0] = from[0];
    to[1] = from[1];
    to[2] = from[2];
}
