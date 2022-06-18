#ifndef UNIFORM_H
#define UNIFORM_H

#include <cstring>
#include <string>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

template <typename T, int D1, int D2>
class Uniform : public Bindable
{
    std::string name;
    T* data;

public:
    Uniform(Graphics& gfx, std::string const& name, T* data);
    void Bind(Graphics& gfx) override;
    void SetData(T* values);
};

template <typename T, int D1, int D2>
Uniform<T, D1, D2>::Uniform(Graphics& gfx, std::string const& name, T* data)
    : name(name)
    , data(data)
{
}

template <typename T, int D1, int D2>
void Uniform<T, D1, D2>::Bind(Graphics& gfx)
{
    gfx.SetUniform<T, D1, D2>(name, data);
}

template <typename T, int D1, int D2>
void Uniform<T, D1, D2>::SetData(T* values)
{
    delete[] data;
    data = new T[D1 * D2];
    std::memcpy(data, values, D1 * D2 * sizeof(T));
}

#endif
