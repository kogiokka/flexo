#ifndef TRANSFORM_UNIFORM_BUFFER_H
#define TRANSFORM_UNIFORM_BUFFER_H

#include <cstring>
#include <string>

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/drawable/Drawable.hpp"

namespace Bind
{
    class TransformUniformBuffer : public Bindable
    {
        struct TransformMatrices {
            glm::mat4 model;
            glm::mat4 viewProj;
        };

        UniformBuffer<TransformMatrices> m_buffer;
        glm::mat4 m_mat;

    public:
        TransformUniformBuffer(Graphics& gfx, glm::mat4 transform);
        void Bind() override;
    };
}

#endif
