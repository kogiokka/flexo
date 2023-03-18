#include "gfx/Vertex.hpp"
#include "log/Logger.h"

#define X(name, size) size,
static unsigned int AttribSize[] = { VERTEX_ATTRIB_TYPES };
#undef X

Vertex::Vertex()
    : m_data(nullptr)
    , m_size(0)
{
}

Vertex::~Vertex()
{
    delete[] m_data;
}

Vertex::Vertex(Vertex const& other)
{
    m_data = new unsigned char[other.m_size];
    std::memcpy(m_data, other.m_data, other.m_size);

    m_struct = other.m_struct;
    m_size = other.m_size;
}

void Vertex::AddAttrib(std::string name, Vertex::Format format)
{
    auto it = m_struct.find(name);
    if (it != m_struct.end()) {
        log_warn("Duplicate element: %s", name.c_str());
        return;
    }

    unsigned int offset = m_size;
    unsigned int size = AttribSize[format];

    m_size += size;
    m_struct.emplace(name, Attrib { format, offset });

    log_debug("Added vertex attribute: (name: %s, offset: %u, size: %u)", name.c_str(), offset, size);
}

void Vertex::FinalizeLayout()
{
    m_data = new unsigned char[m_size];
    log_debug("Vertex struct size: %u", m_size);
}

unsigned int Vertex::GetStride() const
{
    return m_size;
}

unsigned char* Vertex::GetData() const
{
    return m_data;
}
