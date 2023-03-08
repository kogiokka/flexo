#include <cstring>

#include "gfx/bindable/UniformBuffer.hpp"

namespace Bind
{
    UniformBuffer::UniformBuffer(Graphics& gfx, UniformBlock const& ub, std::string id)
        : m_bindex(ub.BIndex())
        , m_id(id)
    {
        GLWRBufferDesc desc;
        GLWRResourceData data;

        desc.type = GLWRResourceType_UniformBuffer;
        desc.usage = GL_STREAM_DRAW;
        desc.byteWidth = ub.Size();
        desc.stride = ub.Size();

        data.mem = ub.Data();
        gfx.CreateBuffer(&desc, &data, &m_buffer);
    }

    void UniformBuffer::Bind(Graphics& gfx)
    {
        gfx.SetUniformBuffers(m_bindex, 1, m_buffer.GetAddressOf());
    }

    void UniformBuffer::Update(Graphics& gfx, UniformBlock const& ub)
    {
        GLWRMappedSubresource mem;
        gfx.Map(m_buffer.Get(), GLWRMapPermission_WriteOnly, &mem);
        std::memcpy(mem.data, ub.Data(), ub.Size());
        gfx.Unmap(m_buffer.Get());
    }

    unsigned int UniformBuffer::BIndex() const
    {
        return m_bindex;
    }

    std::string Bind::UniformBuffer::Id() const
    {
        return m_id;
    }

}
