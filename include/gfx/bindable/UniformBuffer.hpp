#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include <cstring>
#include <string>

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"

namespace Bind
{
    template <typename T>
    class UniformBuffer : public Bindable
    {
        GLWRPtr<IGLWRBuffer> m_buffer;
        GLuint m_bindingIndex;

    public:
        UniformBuffer(Graphics& gfx, GLuint bindingIndex = 0);
        UniformBuffer(Graphics& gfx, T const& uniformBlock, GLuint bindingIndex = 0);
        ~UniformBuffer();
        void Bind(Graphics& gfx) override;
        void Update(Graphics& gfx, T const& uniformBlock);
    };

    template <typename T>
    UniformBuffer<T>::UniformBuffer(Graphics& gfx, GLuint bindingIndex)
        : m_bindingIndex(bindingIndex)
    {
        GLWRBufferDesc desc;
        GLWRResourceData data;

        desc.type = GLWRResourceType_UniformBuffer;
        desc.usage = GL_STREAM_DRAW;
        desc.byteWidth = sizeof(T);
        desc.stride = sizeof(T);

        data.mem = nullptr;
        gfx.CreateBuffer(&desc, &data, &m_buffer);
    }

    template <typename T>
    UniformBuffer<T>::UniformBuffer(Graphics& gfx, T const& uniformBlock, GLuint bindingIndex)
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
    UniformBuffer<T>::~UniformBuffer()
    {
    }

    template <typename T>
    void UniformBuffer<T>::Bind(Graphics& gfx)
    {
        gfx.SetUniformBuffers(m_bindingIndex, 1, m_buffer.GetAddressOf());
    }

    template <typename T>
    void UniformBuffer<T>::Update(Graphics& gfx, T const& uniformBlock)
    {
        GLWRMappedSubresource mem;
        gfx.Map(m_buffer.Get(), GLWRMapPermission_WriteOnly, &mem);
        std::memcpy(mem.data, &uniformBlock, sizeof(T));
        gfx.Unmap(m_buffer.Get());
    }
}

#endif
