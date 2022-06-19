#ifndef CUBE_H
#define CUBE_H

#include <glm/glm.hpp>

#include "Drawable.hpp"

class UVSphere : public Drawable
{
    struct UniformBuffer {
        glm::mat4 viewProjMat;
        glm::mat4 modelMat;
        glm::vec3 viewPos;
        LightUniform light;
        MaterialUniform material;
        float alpha;
    };

    GLuint count_;
    UniformBuffer uniformBuf_;

public:
    UVSphere(Graphics& gfx);
    void Draw(Graphics& gfx) const override;
};

#endif
