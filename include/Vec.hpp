#ifndef VEC_H
#define VEC_H

#include <array>

template <int Dim, typename T = float>
struct Vec {
    template <typename... I>
    explicit Vec(I... args)
        : mData { args... }
    {
    }

    float& operator[](int index)
    {
        return mData[index];
    }

    float const& operator[](int index) const
    {
        return mData[index];
    }

    int Dimension() const
    {
        return Dim;
    }

private:
    std::array<T, Dim> mData;
};

using Vec3f = Vec<3>;
using Vec2f = Vec<2>;

#endif
