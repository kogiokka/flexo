#ifndef VOLUMETRIC_MODEL_H
#define VOLUMETRIC_MODEL_H

#include <memory>

#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "Mesh.hpp"
#include "bindable/Texture2D.hpp"
#include "drawable/Drawable.hpp"

class VolumetricModel : public Drawable
{
    struct UniformBlock {
        struct Vert {
            struct Light {
                STD140_ALIGN glm::vec3 position;
                STD140_ALIGN glm::vec3 ambient;
                STD140_ALIGN glm::vec3 diffusion;
                STD140_ALIGN glm::vec3 specular;
            };

            struct Material {
                STD140_ALIGN glm::vec3 ambient;
                STD140_ALIGN glm::vec3 diffusion;
                STD140_ALIGN glm::vec3 specular;
                float shininess;
            };
            glm::mat4 viewProjMat;
            glm::mat4 modelMat;
            Light light;
            Material material;
            STD140_ALIGN glm::vec3 viewPos;
            float alpha;
        };

        Vert vert;
    };

    GLuint count_;
    UniformBlock ub_;
    std::shared_ptr<Texture2D> texColor_;
    std::shared_ptr<Texture2D> texPattern_;

public:
    VolumetricModel(Graphics& gfx, Mesh const& mesh);
    void Draw(Graphics& gfx) const override;
    void Update(Graphics& gfx) override;
};

#endif
