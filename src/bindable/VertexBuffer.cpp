#include "bindable/VertexBuffer.hpp"
#include "Vertex.hpp"

void VertexBuffer::Bind(Graphics& gfx)
{
    int const numBuffers = offsets_.size();
    std::vector<GLuint> buffers(numBuffers, id_);
    gfx.SetVertexBuffers(0, numBuffers, buffers.data(), offsets_.data());
}
