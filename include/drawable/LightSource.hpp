#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "Mesh.hpp"
#include "drawable/Drawable.hpp"

class LightSource : public Drawable
{
    struct UniformBlock {
        struct Vert {
            glm::mat4 viewProjMat;
            glm::mat4 modelMat;
        };
        struct Frag {
            STD140_ALIGN glm::vec3 lightColor;
        };

        Vert vert;
        Frag frag;
    };

    GLuint count_;
    glm::mat4 scaling_;
    UniformBlock ub_;

public:
    LightSource(Graphics& gfx, Mesh const& mesh);
    void Draw(Graphics& gfx) const override;
    void Update(Graphics& gfx) override;
};

#endif