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

    Vec<Dim, T> operator-(Vec<Dim, T> const& other) const
    {
        Vec<Dim, T> vec;
        for (int d = 0; d < Dim; d++) {
            vec[d] = mData[d] - other[d];
        }
        return vec;
    }

    Vec<Dim, T>& operator+=(Vec<Dim, T> const& other)
    {
        for (int d = 0; d < Dim; d++) {
            mData[d] += other[d];
        }
        return *this;
    }

private:
    std::array<T, Dim> mData;
};

template <int Dim, typename T = float>
Vec<Dim, T> operator*(Vec<Dim, T> const& v, T const& scalar)
{
    Vec<Dim, T> vec;
    for (int d = 0; d < Dim; d++) {
        vec[d] = scalar * v[d];
    }
    return vec;
}

template <int Dim, typename T = float>
Vec<Dim, T> operator*(T const& scalar, Vec<Dim, T> const& v)
{
    Vec<Dim, T> vec;
    for (int d = 0; d < Dim; d++) {
        vec[d] = scalar * v[d];
    }
    return vec;
}

/**
 * Dot product
 */
template <int Dim, typename T = float>
float operator*(Vec<Dim, T> const& v1, Vec<Dim, T> const& v2)
{
    T product = 0;
    for (int d = 0; d < Dim; d++) {
        product += v1[d] * v2[d];
    }
    return product;
}

using Vec3f = Vec<3>;
using Vec2f = Vec<2>;

#endif
