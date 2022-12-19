#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include <cstring>
#include <string>

#include "gfx/Graphics.hpp"
#include "gfx/UniformBlock.hpp"
#include "gfx/bindable/Bindable.hpp"

namespace Bind
{
    class UniformBuffer : public Bindable
    {
        GLWRPtr<IGLWRBuffer> m_buffer;
        GLuint m_bindingIndex;

    public:
        UniformBuffer(Graphics& gfx, UniformBlock const& ub, GLuint bindingIndex = 0);
        ~UniformBuffer() = default;
        void Bind(Graphics& gfx) override;
        void Update(Graphics& gfx, UniformBlock const& ub);
        unsigned int Index() const;
    };
}

#endif
