#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <vector>

#include "Bindable.hpp"
#include "Graphics.hpp"

class VertexBuffer : public Bindable
{
    GLuint id_;
    GLsizei stride_;

public:
    template <typename T>
    VertexBuffer(Graphics& gfx, std::vector<T> const& vertices);
    void Bind(Graphics& gfx) override;
};

#endif
