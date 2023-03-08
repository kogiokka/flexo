#ifndef VEC_UTIL_H
#define VEC_UTIL_H

#include "Vec.hpp"
#include <glm/glm.hpp>

inline glm::vec2 VECCONV(Vec2f v)
{
    return { v.x, v.y };
}

inline glm::vec3 VECCONV(Vec3f v)
{
    return { v.x, v.y, v.z };
}

inline Vec2f VECCONV(glm::vec2 v)
{
    return { v.x, v.y };
}

inline Vec3f VECCONV(glm::vec3 v)
{
    return { v.x, v.y, v.z };
}

#endif
