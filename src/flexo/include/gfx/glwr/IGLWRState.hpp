#ifndef I_GLWR_STATE_H
#define I_GLWR_STATE_H

#include <functional>
#include <vector>

#include "gfx/glwr/IGLWRBase.hpp"

class Graphics;

class IGLWRState : public IGLWRBase
{
    friend Graphics;
    using _Op = std::function<void(void)>;
    std::vector<_Op> m_ops;

private:
    IGLWRState() = default;
    ~IGLWRState() override = default;
    void Add(_Op op);
    void Execute() const;
};

using IGLWRRasterizerState = IGLWRState;
using IGLWRBlendState = IGLWRState;

#endif
