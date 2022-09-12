#include "gfx/glwr/IGLWRState.hpp"

void IGLWRState::Add(_Op op)
{
    m_ops.push_back(op);
}

void IGLWRState::Execute() const
{
    for (auto const& op : m_ops) {
        op();
    }
}
