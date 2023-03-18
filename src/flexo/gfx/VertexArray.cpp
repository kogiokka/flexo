#include "gfx/VertexArray.hpp"
#include "log/Logger.h"

#define X(name, size) size,
static unsigned int AttribSize[] = { VERTEX_ATTRIB_FORMAT };
#undef X

void VertexLayout::AddAttrib(std::string name, VertexLayout::Attrib::Format format)
{
    auto it = m_attrs.find(name);
    if (it != m_attrs.end()) {
        log_warn("Duplicate vertex attribute: %s", name.c_str());
        return;
    }

    unsigned int offset = m_size;
    unsigned int size = AttribSize[format];

    m_size += size;
    m_attrs.emplace(name, Attrib { format, offset });

    log_debug("Added vertex attribute: (name: %s, offset: %u, size: %u)", name.c_str(), offset, size);
}

int VertexLayout::GetOffset(std::string const& name) const
{
    auto it = m_attrs.find(name);
    if (it == m_attrs.end()) {
        return -1;
    }

    return it->second.offset;
}

unsigned int VertexLayout::GetSize() const
{
    return m_size;
}

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
