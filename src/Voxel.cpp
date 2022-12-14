#include "Voxel.hpp"

Voxel::Voxel(glm::vec3 pos, glm::vec2 uv, Voxel::Vis vis)
    : pos(pos)
    , uv(uv)
    , vis(vis)
{
}
