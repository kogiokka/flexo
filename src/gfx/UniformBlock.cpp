#include "gfx/UniformBlock.hpp"

#define X(name, size, padded) size,
static unsigned int ElementSize[] = { UNIFORM_ELEMENT_TYPES };
#undef X

#define X(name, size, padded) padded,
static unsigned int ElementPaddedSize[] = { UNIFORM_ELEMENT_TYPES };
#undef X

UniformBlock::UniformBlock()
    : m_size(0)
    , m_buffer(nullptr)
    , m_layout()
{
}

UniformBlock::~UniformBlock()
{
    delete[] m_buffer;
}

void UniformBlock::AddElement(UniformBlock::Type type, std::string name)
{
    Element elem;
    elem.name = name;
    elem.type = type;
    m_struct.push_back(elem);
}

void UniformBlock::FinalizeLayout()
{
    unsigned int largest = 0;

    for (auto const& m : m_struct) {
        auto size = ElementPaddedSize[m.type];
        if (size > largest) {
            largest = size;
        }
    }

    unsigned int remain = 0;
    for (auto m : m_struct) {
        unsigned int size = ElementSize[m.type];
        if (size > remain) {
            m.begin = m_size + remain; // add the padding
            m_size += (remain + size); // new size
            remain = largest - size; // update remain
        } else {
            m.begin = m_size;
            m_size += size;
        }

        auto const& [it, success] = m_layout.emplace(m.name, m);
        if (!success) {
            log_warn("Duplicate element: \"%s\".", m.name.c_str());
            continue;
        }
    }

    log_debug("Uniform block size: %u", m_size);
    m_buffer = new unsigned char[m_size];
}

unsigned int UniformBlock::Size() const
{
    return m_size;
}

void const* UniformBlock::Data() const
{
    return m_buffer;
}
