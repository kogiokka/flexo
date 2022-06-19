#ifndef UNIFORM_H
#define UNIFORM_H

#include <cstring>
#include <string>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

template <typename T>
class UniformBuffer : public Bindable
{
public:
    UniformBuffer(Graphics& gfx, T const& uniform);
    void Bind(Graphics& gfx) override;
};

template <typename T>
UniformBuffer<T>::UniformBuffer(Graphics& gfx, const T& uniform)
{
}

template <typename T>
void UniformBuffer<T>::Bind(Graphics& gfx)
{
}

#endif
