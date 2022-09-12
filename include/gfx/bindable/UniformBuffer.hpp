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
        void Bind() override;
        void Update(T const& uniformBlock);
    };

    template <typename T>
    UniformBuffer<T>::UniformBuffer(Graphics& gfx, GLuint bindingIndex)
        : Bindable(gfx)
        , m_bindingIndex(bindingIndex)
    {
        GLWRBufferDesc desc;
        GLWRResourceData data;

        desc.target = GL_UNIFORM_BUFFER;
        desc.usage = GL_STREAM_DRAW;
        desc.byteWidth = sizeof(T);
        desc.stride = sizeof(T);

        data.mem = nullptr;
        m_gfx->CreateBuffer(&desc, &data, &m_buffer);
    }

    template <typename T>
    UniformBuffer<T>::UniformBuffer(Graphics& gfx, T const& uniformBlock, GLuint bindingIndex)
        : Bindable(gfx)
        , m_bindingIndex(bindingIndex)
    {
        GLWRBufferDesc desc;
        GLWRResourceData data;

        desc.target = GL_UNIFORM_BUFFER;
        desc.usage = GL_STREAM_DRAW;
        desc.byteWidth = sizeof(T);
        desc.stride = sizeof(T);

        data.mem = &uniformBlock;
        m_gfx->CreateBuffer(&desc, &data, &m_buffer);
    }

    template <typename T>
    UniformBuffer<T>::~UniformBuffer()
    {
    }

    template <typename T>
    void UniformBuffer<T>::Bind()
    {
        m_gfx->SetUniformBuffer(m_bindingIndex, m_buffer.Get());
    }

    template <typename T>
    void UniformBuffer<T>::Update(T const& uniformBlock)
    {
        GLWRResourceData data;
        data.mem = &uniformBlock;
        m_gfx->UpdateBuffer(GL_UNIFORM_BUFFER, 0, sizeof(T), &data, m_buffer.Get());
    }
}

#endif
