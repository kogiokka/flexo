#ifndef UNIFORM_BLOCK_H
#define UNIFORM_BLOCK_H

#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

#include "util/Logger.h"

// std140 layout rules
#define UNIFORM_ELEMENT_TYPES                                                                                          \
    X(bool32, 4, 4)                                                                                                    \
    X(i32, 4, 4)                                                                                                       \
    X(u32, 4, 4)                                                                                                       \
    X(f32, 4, 4)                                                                                                       \
    X(vec2f32, 8, 8)                                                                                                   \
    X(vec3f32, 12, 16)                                                                                                 \
    X(vec4f32, 16, 16)                                                                                                 \
    X(mat4, 64, 64)

class UniformBlock
{
public:
#define X(name, size, padded) name,
    enum Type { UNIFORM_ELEMENT_TYPES };
#undef X

private:
    struct Element {
        std::string name;
        Type type;
        unsigned int begin;
    };

public:
    UniformBlock();

    UniformBlock(UniformBlock const& other);
    UniformBlock& operator=(UniformBlock const& other);
    ~UniformBlock();

    void AddElement(UniformBlock::Type type, std::string name);
    void SetBIndex(unsigned int index);
    void FinalizeLayout();
    unsigned int Size() const;
    void const* Data() const;
    unsigned int BIndex() const;

    template <typename T>
    void Assign(std::string const& name, T const& value);

private:
    unsigned int m_bindex;
    unsigned int m_size;
    unsigned char* m_buffer;
    std::vector<Element> m_struct;
    std::unordered_map<std::string, Element> m_layout;
};

template <typename T>
void UniformBlock::Assign(std::string const& name, T const& value)
{
    if (!m_buffer) {
        log_error("Layout has not been finalized.");
        return;
    }

    auto it = m_layout.find(name);
    if (it == m_layout.end()) {
        log_error("Layout element \"%s\" does not exists.", name.c_str());
        return;
    }

    Element const& elem = it->second;
    std::memcpy(&m_buffer[elem.begin], &value, sizeof(T));
}

#endif
