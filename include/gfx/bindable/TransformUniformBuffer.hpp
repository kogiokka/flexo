#ifndef TRANSFORM_UNIFORM_BUFFER_H
#define TRANSFORM_UNIFORM_BUFFER_H

#include <cstring>
#include <memory>
#include <string>

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/drawable/Drawable.hpp"

namespace Bind
{
    class TransformUniformBuffer : public Bindable
    {
        glm::mat4 m_mat;
        UniformBlock m_ub;
        std::unique_ptr<UniformBuffer> m_buffer;

    public:
        TransformUniformBuffer(Graphics& gfx, glm::mat4 transform);
        void Bind(Graphics& gfx) override;
    };
}

#endif
