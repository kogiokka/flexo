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
        GLuint id_;
        GLuint bindingIndex_;

    public:
        UniformBuffer(Graphics& gfx, T const& uniformBlock);
        ~UniformBuffer();
        void Bind() override;
        void Update(T const& uniformBlock);
    };

    template <typename T>
    UniformBuffer<T>::UniformBuffer(Graphics& gfx, T const& uniformBlock)
        : Bindable(gfx)
        , id_(0)
        , bindingIndex_(0)
    {
        BufferDesc desc;
        BufferData data;

        desc.target = GL_UNIFORM_BUFFER;
        desc.usage = GL_STREAM_DRAW;
        desc.byteWidth = sizeof(T);
        desc.stride = sizeof(T);

        data.mem = &uniformBlock;
        gfx_->CreateBuffer(id_, desc, data);
    }

    template <typename T>
    UniformBuffer<T>::~UniformBuffer()
    {
        gfx_->DeleteBuffer(id_);
    }

    template <typename T>
    void UniformBuffer<T>::Bind()
    {
        gfx_->SetUniformBuffer(id_, bindingIndex_);
    }

    template <typename T>
    void UniformBuffer<T>::Update(T const& uniformBlock)
    {
        gfx_->UpdateBuffer(id_, GL_UNIFORM_BUFFER, 0, sizeof(T), &uniformBlock);
    }
}

#endif
