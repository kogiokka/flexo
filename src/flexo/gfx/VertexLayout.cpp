#include <cstdlib>

#include "gfx/VertexLayout.hpp"
#include "log/Logger.h"

#define X(name, size) size,
static unsigned int AttribSize[] = { VERTEX_ATTRIB_FORMAT };
#undef X

void VertexLayout::AddAttrib(std::string name, VertexLayout::AttribFormat format)
{
    auto it = m_attrs.find(name);
    if (it != m_attrs.end()) {
        log_warn("Duplicate vertex attribute: %s", name.c_str());
        return;
    }

    unsigned int offset = m_size;
    unsigned int size = AttribSize[format];

    m_size += size;
    m_attrs.emplace(name, Attrib { offset });

    log_debug("Added vertex attribute: (name: %s, offset: %u, size: %u)", name.c_str(), offset, size);
}

unsigned int VertexLayout::GetOffset(std::string const& name) const
{
    auto const& it = m_attrs.find(name);
    if (it == m_attrs.end()) {
        log_fatal("Vertex attribute \"%s\" does not exist.", name.c_str());
        exit(EXIT_FAILURE);
    }

    return it->second.offset;
}

unsigned int VertexLayout::GetSize() const
{
    return m_size;
}
