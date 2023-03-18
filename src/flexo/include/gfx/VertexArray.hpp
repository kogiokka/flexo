#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

#include "log/Logger.h"

#define VERTEX_ATTRIB_FORMAT                                                                                           \
    X(Float, 4)                                                                                                        \
    X(Float2, 8)                                                                                                       \
    X(Float3, 12)                                                                                                      \
    X(Float4, 16)

class VertexLayout
{
public:
#define X(name, size) name,
    enum AttribFormat { VERTEX_ATTRIB_FORMAT };
#undef X
    struct Attrib {
        unsigned int offset;
    };

public:
    void AddAttrib(std::string name, VertexLayout::AttribFormat format);
    int GetOffset(std::string const& name) const;
    unsigned int GetSize() const;

private:
    unsigned int m_size = 0;
    std::unordered_map<std::string, Attrib> m_attrs;
};

class VertexArray
{
public:
    explicit VertexArray(VertexLayout layout);
    template <typename T>
    void Assign(std::string const& name, T const& value);
    void PushBack();

    unsigned int GetStride() const;
    unsigned int GetCount() const;
    unsigned char const* GetData() const;

private:
    std::vector<unsigned char> m_array;
    std::vector<unsigned char> m_buf;
    VertexLayout m_layout;
    unsigned int m_count = 0;
};

template <typename T>
void VertexArray::Assign(std::string const& name, T const& value)
{
    int offset = m_layout.GetOffset(name);
    if (offset < 0) {
        log_error("Vertex attribute \"%s\" does not exist.", name.c_str());
        return;
    }

    std::memcpy(&m_buf[offset], &value, sizeof(T));
}

#endif
