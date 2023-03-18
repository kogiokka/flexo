#ifndef VERTEX_H
#define VERTEX_H

#include <cstring>
#include <string>
#include <unordered_map>

#include "log/Logger.h"

#define VERTEX_ATTRIB_TYPES                                                                                            \
    X(i32, 4)                                                                                                          \
    X(u32, 4)                                                                                                          \
    X(f32, 4)                                                                                                          \
    X(vec2f32, 8)                                                                                                      \
    X(vec3f32, 12)                                                                                                     \
    X(vec4f32, 16)

class Vertex
{
public:
#define X(name, size) name,
    enum Format { VERTEX_ATTRIB_TYPES };
#undef X

private:
    struct Attrib {
        Format format;
        unsigned int offset;
    };

public:
    Vertex();
    ~Vertex();
    Vertex(Vertex const& other);
    void AddAttrib(std::string name, Vertex::Format format);
    void FinalizeLayout();
    template <typename T>
    void Assign(std::string const& name, T const& value);
    unsigned int GetStride() const;
    unsigned char* GetData() const;

private:
    unsigned char* m_data;
    unsigned int m_size;
    std::unordered_map<std::string, Attrib> m_struct;
};

template <typename T>
void Vertex::Assign(std::string const& name, T const& value)
{
    if (!m_data) {
        log_error("Layout has not been finalized.");
        return;
    }

    auto it = m_struct.find(name);
    if (it == m_struct.end()) {
        log_error("Vertex attribute \"%s\" does not exist.", name.c_str());
        return;
    }

    std::memcpy(&m_data[it->second.offset], &value, sizeof(T));
}

#endif
