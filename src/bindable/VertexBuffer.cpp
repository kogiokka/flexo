#include "bindable/VertexBuffer.hpp"
#include "Vertex.hpp"

namespace Bind
{
    void VertexBuffer::Bind(Graphics& gfx)
    {
        gfx.SetVertexBuffers(startAttrib_, numAttrs_, std::vector<GLuint>(numAttrs_, id_).data(), offsets_.data(),
                             strides_.data());
    }

    GLuint VertexBuffer::GetStartAttrib() const
    {
        return startAttrib_;
    }

    GLuint VertexBuffer::GetCount() const
    {
        return count_;
    }

    template <>
    void VertexBuffer::SetAttribMemLayout<VertexPN>()
    {
        numAttrs_ = 2;
        offsets_ = { offsetof(VertexPN, position), offsetof(VertexPN, normal) };
        strides_ = std::vector<GLsizei>(numAttrs_, sizeof(VertexPN));
    }

    template <>
    void VertexBuffer::SetAttribMemLayout<VertexPNT>()
    {
        numAttrs_ = 3;
        offsets_ = { offsetof(VertexPNT, position), offsetof(VertexPNT, normal), offsetof(VertexPNT, texcoord) };
        strides_ = std::vector<GLsizei>(numAttrs_, sizeof(VertexPNT));
    }

    template <>
    void VertexBuffer::SetAttribMemLayout<VertexPNC>()
    {
        numAttrs_ = 3;
        offsets_ = { offsetof(VertexPNC, position), offsetof(VertexPNC, normal), offsetof(VertexPNC, color) };
        strides_ = std::vector<GLsizei>(numAttrs_, sizeof(VertexPNC));
    }

    template <>
    void VertexBuffer::SetAttribMemLayout<glm::vec3>()
    {
        numAttrs_ = 1;
        offsets_ = { 0 };
        strides_ = std::vector<GLsizei>(numAttrs_, sizeof(glm::vec3));
    }

    template <>
    void VertexBuffer::SetAttribMemLayout<glm::vec2>()
    {
        numAttrs_ = 1;
        offsets_ = { 0 };
        strides_ = std::vector<GLsizei>(numAttrs_, sizeof(glm::vec2));
    }
}
