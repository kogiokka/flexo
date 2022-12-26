#ifndef VEC_H
#define VEC_H

#include <array>

template <int Dim, typename T = float>
struct Vec {
    template <typename... I>
    Vec(I... args);
    T& operator[](int index);
    T const& operator[](int index) const;
    int Dimension() const;

    Vec<Dim, T> operator-(Vec<Dim, T> const& other) const;
    Vec<Dim, T>& operator+=(Vec<Dim, T> const& other);

    std::array<T, Dim> mData;
};

template <typename T>
struct Vec<2, T> {
    Vec();
    Vec(T x, T y);
    T& operator[](int index);
    T const& operator[](int index) const;
    int Dimension() const;

    Vec<2, T> operator-(Vec<2, T> const& other) const;
    Vec<2, T>& operator+=(Vec<2, T> const& other);

    T x, y;
};

template <typename T>
struct Vec<3, T> {
    Vec();
    Vec(T x, T y, T z);
    T& operator[](int index);
    T const& operator[](int index) const;
    int Dimension() const;

    Vec<3, T> operator-(Vec<3, T> const& other) const;
    Vec<3, T>& operator+=(Vec<3, T> const& other);

    T x, y, z;
};

template <int Dim, typename T = float>
Vec<Dim, T> operator*(Vec<Dim, T> const& v, T const& scalar);

template <int Dim, typename T = float>
Vec<Dim, T> operator*(T const& scalar, Vec<Dim, T> const& v);

/**
 * Dot product
 */
template <int Dim, typename T = float>
T operator*(Vec<Dim, T> const& v1, Vec<Dim, T> const& v2);

template <typename T>
using Vec2 = Vec<2, T>;

template <typename T>
using Vec3 = Vec<3, T>;

using Vec2i = Vec2<int>;
using Vec3i = Vec3<int>;
using Vec2f = Vec2<float>;
using Vec3f = Vec3<float>;

#include "Vec.inl"
#endif

