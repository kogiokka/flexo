#ifndef VERTEX_LAYOUT_H
#define VERTEX_LAYOUT_H

#include <string>
#include <unordered_map>

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
    unsigned int GetOffset(std::string const& name) const;
    unsigned int GetSize() const;

private:
    unsigned int m_size = 0;
    std::unordered_map<std::string, Attrib> m_attrs;
};

#endif
