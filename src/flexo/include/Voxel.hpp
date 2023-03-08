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

typedef unsigned char VoxelVis;

enum VoxelVis_ : unsigned char {
    VoxelVis_None = 0b0000'0000,
    VoxelVis_XPos = 0b0000'0001,
    VoxelVis_XNeg = 0b0000'0010,
    VoxelVis_YPos = 0b0000'0100,
    VoxelVis_YNeg = 0b0000'1000,
    VoxelVis_ZPos = 0b0001'0000,
    VoxelVis_ZNeg = 0b0010'0000,
    VoxelVis_All = 0b0011'1111,
};

struct Voxel {
    Voxel(glm::vec3 pos, glm::vec2 uv, VoxelVis vis = VoxelVis_All);

    glm::vec3 pos;
    glm::vec2 uv;
    VoxelVis vis;
};

#endif
