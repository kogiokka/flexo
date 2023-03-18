#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <cstring>
#include <string>
#include <vector>

#include "gfx/VertexLayout.hpp"
#include "log/Logger.h"

class VertexArray
{
public:
    explicit VertexArray(VertexLayout layout);
    template <typename T>
    void Assign(std::string const& name, T const& value);
    void PushBack();

    VertexLayout const& GetLayout() const;
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
