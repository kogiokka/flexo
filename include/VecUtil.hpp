#ifndef VEC_UTIL_H
#define VEC_UTIL_H

#include "Vec.hpp"
#include <glm/glm.hpp>

void SetVec3(Vec<2, float>& to, glm::vec2 const& from);
void SetVec3(Vec<3, float>& to, glm::vec3 const& from);
void SetVec2(glm::vec2& to, Vec<2, float> const& from);
void SetVec2(glm::vec3& to, Vec<3, float> const& from);

#endif
