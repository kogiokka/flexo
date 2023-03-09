#include "gfx/VertexBuffer.hpp"

VertexBuffer::VertexBuffer(Mesh const& mesh)
    : m_data(nullptr)
    , m_count(0)
    , m_stride(0)
{
    unsigned int size = 0;
    unsigned int szp = 0;
    unsigned int szn = 0;
    unsigned int szt = 0;

    if (mesh.HasPositions()) {
        szp = sizeof(mesh.positions.front());
        size += mesh.positions.size() * szp;
        m_stride += szp;
    }
    if (mesh.HasNormals()) {
        szn = sizeof(mesh.normals.front());
        size += mesh.normals.size() * szn;
        m_stride += szn;
    }
    if (mesh.HasTextureCoords()) {
        szt = sizeof(mesh.textureCoords.front());
        size += mesh.textureCoords.size() * szt;
        m_stride += szt;
    }

    m_data = new unsigned char[size];
    m_count = mesh.positions.size();

    unsigned int offset = 0;
    for (unsigned int i = 0; i < mesh.positions.size(); i++) {
        if (mesh.HasPositions()) {
            memcpy(m_data + offset, &mesh.positions[i][0], szp);
            offset += szp;
        }
        if (mesh.HasNormals()) {
            memcpy(m_data + offset, &mesh.normals[i][0], szn);
            offset += szn;
        }
        if (mesh.HasTextureCoords()) {
            memcpy(m_data + offset, &mesh.textureCoords[i][0], szt);
            offset += szt;
        }
    }

    m_szPos = szp;
    m_szNorm = szn;
    m_szTex = szt;
}

unsigned int VertexBuffer::Stride() const
{
    return m_stride;
}

unsigned int VertexBuffer::Count() const
{
    return m_count;
}

const void* VertexBuffer::Data() const
{
    return m_data;
}

VertexBuffer::~VertexBuffer()
{
    delete m_data;
}

unsigned int VertexBuffer::OffsetOfPosition()
{
    return 0;
}

unsigned int VertexBuffer::OffsetOfNormal()
{
    return m_szPos;
}

unsigned int VertexBuffer::OffsetOfTextureCoords()
{
    return m_szPos + m_szNorm;
}
