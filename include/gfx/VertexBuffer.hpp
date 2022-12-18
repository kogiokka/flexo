#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <cstring>

#include "Mesh.hpp"

class VertexBuffer
{
public:
    explicit VertexBuffer(Mesh const& mesh);
    template <typename T>
    explicit VertexBuffer(std::vector<T> data);
    ~VertexBuffer();

    unsigned int OffsetOfPosition();
    unsigned int OffsetOfNormal();
    unsigned int OffsetOfTextureCoords();

    unsigned int Stride() const;
    unsigned int Count() const;
    void const* Data() const;

private:
    unsigned char* m_data;
    unsigned int m_count;
    unsigned int m_stride;

    unsigned int m_szPos;
    unsigned int m_szNorm;
    unsigned int m_szTex;
};

template <typename T>
VertexBuffer::VertexBuffer(std::vector<T> data)
{
    unsigned int stride = sizeof(T);
    unsigned int size = stride * data.size();

    m_stride = stride;
    m_data = new unsigned char[size];
    m_count = data.size();

    unsigned int offset = 0;
    for (unsigned int i = 0; i < data.size(); i++) {
        mempcpy(m_data + offset, &data[i][0], stride);
        offset += stride;
    }

    m_szPos = 0;
    m_szNorm = 0;
    m_szTex = 0;
}

#endif
