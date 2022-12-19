#include "gfx/bindable/UniformBuffer.hpp"

namespace Bind
{
    void UniformBuffer::Bind(Graphics& gfx)
    {
        gfx.SetUniformBuffers(m_bindingIndex, 1, m_buffer.GetAddressOf());
    }
}
