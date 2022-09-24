#ifndef VEC_H
#define VEC_H

#include <array>

template <int Dim, typename T = float>
struct Vec {
    template <typename... I>
    explicit Vec(I... args);
    float& operator[](int index);
    float const& operator[](int index) const;
    int Dimension() const;

    Vec<Dim, T> operator-(Vec<Dim, T> const& other) const;
    Vec<Dim, T>& operator+=(Vec<Dim, T> const& other);

private:
    std::array<T, Dim> mData;
};

template <int Dim, typename T = float>
Vec<Dim, T> operator*(Vec<Dim, T> const& v, T const& scalar);

template <int Dim, typename T = float>
Vec<Dim, T> operator*(T const& scalar, Vec<Dim, T> const& v);

/**
 * Dot product
 */
template <int Dim, typename T = float>
float operator*(Vec<Dim, T> const& v1, Vec<Dim, T> const& v2);

using Vec3f = Vec<3>;
using Vec2f = Vec<2>;

#include "Vec.inl"
#endif
