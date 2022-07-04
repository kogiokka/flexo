#ifndef TRANSFORM_UNIFORM_BUFFER_H
#define TRANSFORM_UNIFORM_BUFFER_H

#include <cstring>
#include <string>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"
#include "bindable/UniformBuffer.hpp"
#include "drawable/Drawable.hpp"

namespace Bind
{
    class TransformUniformBuffer : public Bindable
    {
        struct TransformMatrices {
            glm::mat4 model;
            glm::mat4 viewProj;
        };

        UniformBuffer<TransformMatrices> m_buffer;
        Drawable const& m_parent;

    public:
        TransformUniformBuffer(Graphics& gfx, Drawable const& parent);
        void Bind() override;
    };
}

#endif
