#ifndef VOLUMETRIC_MODEL_DATA_H
#define VOLUMETRIC_MODEL_DATA_H

#include <string>

#include <glm/glm.hpp>
#include <rvl.h>

class VolumetricModelData
{

public:
    VolumetricModelData();
    ~VolumetricModelData();
    void Read(std::string const filename);
    unsigned char const* GetBuffer();
    glm::ivec3 GetResolution();
    glm::vec3 GetVoxelDims();
    glm::vec3 GetGridOrigin();

private:
    glm::ivec3 m_resolution;
    glm::vec3 m_vxDims;
    glm::vec3 m_origin;
    unsigned char* m_data;
    RVL* m_rvl;
};

#endif
