#ifndef VOXEL_H
#define VOXEL_H

#include <glm/glm.hpp>

/**
 * Voxel Visibility
 *
 * +X  0000 0001
 * -X  0000 0010
 *
 * +Y  0000 0100
 * -Y  0000 1000
 *
 * +Z  0001 0000
 * -Z  0010 0000
 */

struct Voxel {
    enum Vis : unsigned char {
        None = 0b0000'0000,
        XP = 0b0000'0001,
        XN = 0b0000'0010,
        YP = 0b0000'0100,
        YN = 0b0000'1000,
        ZP = 0b0001'0000,
        ZN = 0b0010'0000,
        All = 0b0011'1111,
    };

    Voxel(glm::vec3 pos, glm::vec2 uv, Voxel::Vis vis = Voxel::Vis::All);

    glm::vec3 pos;
    glm::vec2 uv;
    Voxel::Vis vis;
};

#endif

