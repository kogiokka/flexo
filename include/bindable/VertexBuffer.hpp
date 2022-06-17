#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <vector>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

class VertexBuffer : public Bindable
{
    GLuint id_;
    std::vector<GLsizei> strides_;
    std::vector<GLintptr> offsets_;

public:
    template <typename T>
    VertexBuffer(Graphics& gfx, std::vector<T> const& vertices);
    void Bind(Graphics& gfx) override;
};

#endif
