#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include <cstring>
#include <string>

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"

namespace Bind
{
    class UniformBuffer : public Bindable
    {
        GLWRPtr<IGLWRBuffer> m_buffer;
        GLuint m_bindingIndex;

    public:
        template <typename T>
        UniformBuffer(Graphics& gfx, T const& uniformBlock, GLuint bindingIndex = 0);
        ~UniformBuffer() = default;
        void Bind(Graphics& gfx) override;
        template <typename T>
        void Update(Graphics& gfx, T const& uniformBlock);
    };

    template <typename T>
    UniformBuffer::UniformBuffer(Graphics& gfx, T const& uniformBlock, GLuint bindingIndex)
        : m_bindingIndex(bindingIndex)
    {
        GLWRBufferDesc desc;
        GLWRResourceData data;

        desc.type = GLWRResourceType_UniformBuffer;
        desc.usage = GL_STREAM_DRAW;
        desc.byteWidth = sizeof(T);
        desc.stride = sizeof(T);

        data.mem = &uniformBlock;
        gfx.CreateBuffer(&desc, &data, &m_buffer);
    }

    template <typename T>
    void UniformBuffer::Update(Graphics& gfx, T const& uniformBlock)
    {
        GLWRMappedSubresource mem;
        gfx.Map(m_buffer.Get(), GLWRMapPermission_WriteOnly, &mem);
        std::memcpy(mem.data, &uniformBlock, sizeof(T));
        gfx.Unmap(m_buffer.Get());
    }
}

#endif
