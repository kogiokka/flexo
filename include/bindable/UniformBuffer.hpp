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
        void Bind(Graphics& gfx) override;
        void Update(Graphics& gfx, T const& uniformBlock);
    };

    template <typename T>
    UniformBuffer<T>::UniformBuffer(Graphics& gfx, T const& uniformBlock)
        : id_(0)
        , bindingIndex_(0)
    {
        BufferDesc desc;
        BufferData data;

        desc.target = GL_UNIFORM_BUFFER;
        desc.usage = GL_STREAM_DRAW;
        desc.byteWidth = sizeof(T);
        desc.stride = sizeof(T);

        data.mem = &uniformBlock;
        gfx.CreateBuffer(id_, desc, data);
    }

    template <typename T>
    void UniformBuffer<T>::Bind(Graphics& gfx)
    {
        gfx.SetUniformBuffer(id_, bindingIndex_);
    }

    template <typename T>
    void UniformBuffer<T>::Update(Graphics& gfx, T const& uniformBlock)
    {
        gfx.UpdateUniformBuffer<T>(id_, uniformBlock);
    }
}

#endif
