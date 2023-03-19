#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include <string>

#include "gfx/Graphics.hpp"
#include "gfx/UniformBlock.hpp"
#include "gfx/bindable/Bindable.hpp"

namespace Bind
{
    class UniformBuffer : public Bindable
    {
    public:
        UniformBuffer(Graphics& gfx, UniformBlock const& ub, std::string id);
        ~UniformBuffer() = default;
        void Bind(Graphics& gfx) override;
        void Update(Graphics& gfx, UniformBlock const& ub);
        unsigned int BIndex() const;
        std::string Id() const;

    private:
        GLWRPtr<IGLWRBuffer> m_buffer;
        GLuint m_bindex;
        std::string m_id;
    };
}

#endif
