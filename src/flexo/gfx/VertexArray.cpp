#include "gfx/VertexArray.hpp"

VertexArray::VertexArray(VertexLayout layout)
{
    m_layout = layout;
    m_buf = std::vector<unsigned char>(m_layout.GetSize());
}

void VertexArray::PushBack()
{
    m_array.insert(m_array.end(), m_buf.begin(), m_buf.end());
    m_count += 1;
}

VertexLayout const& VertexArray::GetLayout() const
{
    return m_layout;
}

unsigned int VertexArray::GetCount() const
{
    return m_count;
}

unsigned int VertexArray::GetStride() const
{
    return m_layout.GetSize();
}

unsigned char const* VertexArray::GetData() const
{
    return m_array.data();
}
