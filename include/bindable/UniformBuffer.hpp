#ifndef UNIFORM_H
#define UNIFORM_H

#include <cstring>
#include <string>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

namespace Bind
{
    template <typename T>
    class UniformBuffer : public Bindable
    {
        GLuint m_id;
        GLuint m_bindingIndex;

    public:
        UniformBuffer(Graphics& gfx, T const& uniformBlock);
        ~UniformBuffer();
        void Bind() override;
        void Update(T const& uniformBlock);
    };

    template <typename T>
    UniformBuffer<T>::UniformBuffer(Graphics& gfx, T const& uniformBlock)
        : Bindable(gfx)
        , m_id(0)
        , m_bindingIndex(0)
    {
        BufferDesc desc;
        BufferData data;

        desc.target = GL_UNIFORM_BUFFER;
        desc.usage = GL_STREAM_DRAW;
        desc.byteWidth = sizeof(T);
        desc.stride = sizeof(T);

        data.mem = &uniformBlock;
        m_gfx->CreateBuffer(m_id, desc, data);
    }

    template <typename T>
    UniformBuffer<T>::~UniformBuffer()
    {
        m_gfx->DeleteBuffer(m_id);
    }

    template <typename T>
    void UniformBuffer<T>::Bind()
    {
        m_gfx->SetUniformBuffer(m_id, m_bindingIndex);
    }

    template <typename T>
    void UniformBuffer<T>::Update(T const& uniformBlock)
    {
        m_gfx->UpdateBuffer(m_id, GL_UNIFORM_BUFFER, 0, sizeof(T), &uniformBlock);
    }
}

#endif
