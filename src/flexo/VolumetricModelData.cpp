#include <cstdlib>

#include "VolumetricModelData.hpp"
#include "log/Logger.h"

VolumetricModelData::VolumetricModelData()
{
    m_rvl = rvl_create_reader();
}

VolumetricModelData::~VolumetricModelData()
{
    rvl_destroy(&m_rvl);
}

void VolumetricModelData::Read(std::string const filename)
{
    rvl_set_file(m_rvl, filename.c_str());
    rvl_read_rvl(m_rvl);

    RVLenum primitive;
    RVLenum endian;

    glm::ivec3 res;
    glm::vec3 dims;
    glm::vec3 orig;
    rvl_get_volumetric_format(m_rvl, &res.x, &res.y, &res.z, &primitive, &endian);

    if ((primitive != RVL_PRIMITIVE_U8) || rvl_get_grid_type(m_rvl) != RVL_GRID_REGULAR) {
        log_error("Wrong type of volumetric data!");
        exit(EXIT_FAILURE);
    }

    rvl_get_voxel_dims(m_rvl, &dims.x, &dims.y, &dims.z);
    rvl_get_grid_origin(m_rvl, &orig.x, &orig.y, &orig.z);

    m_resolution = res;
    m_vxDims = dims;
    m_origin = orig;
}

unsigned char const* VolumetricModelData::GetBuffer()
{
    void const* buf = rvl_get_voxels(m_rvl);
    return static_cast<unsigned char const*>(buf);
}

glm::ivec3 VolumetricModelData::GetResolution()
{
    return m_resolution;
}

glm::vec3 VolumetricModelData::GetVoxelDims()
{
    return m_vxDims;
}

glm::vec3 VolumetricModelData::GetGridOrigin()
{
    return m_origin;
}
