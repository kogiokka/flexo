#include "Vec.hpp"

template <int Dim, typename T>
template <typename... I>
Vec<Dim, T>::Vec(I... args)
    : mData { args... }
{
}

template <int Dim, typename T>
T& Vec<Dim, T>::operator[](int index)
{
    return mData[index];
}

template <int Dim, typename T>
T const& Vec<Dim, T>::operator[](int index) const
{
    return mData[index];
}

template <int Dim, typename T>
int Vec<Dim, T>::Dimension() const
{
    return Dim;
}

template <int Dim, typename T>
Vec<Dim, T> Vec<Dim, T>::operator-(Vec<Dim, T> const& other) const
{
    Vec<Dim, T> vec;
    for (int d = 0; d < Dim; d++) {
        vec[d] = mData[d] - other[d];
    }
    return vec;
}

template <int Dim, typename T>
Vec<Dim, T>& Vec<Dim, T>::operator+=(Vec<Dim, T> const& other)
{
    for (int d = 0; d < Dim; d++) {
        mData[d] += other[d];
    }
    return *this;
}

template <int Dim, typename T>
Vec<Dim, T> operator*(Vec<Dim, T> const& v, T const& scalar)
{
    Vec<Dim, T> vec;
    for (int d = 0; d < Dim; d++) {
        vec[d] = scalar * v[d];
    }
    return vec;
}

template <int Dim, typename T>
Vec<Dim, T> operator*(T const& scalar, Vec<Dim, T> const& v)
{
    Vec<Dim, T> vec;
    for (int d = 0; d < Dim; d++) {
        vec[d] = scalar * v[d];
    }
    return vec;
}

template <int Dim, typename T>
T operator*(Vec<Dim, T> const& v1, Vec<Dim, T> const& v2)
{
    T product = 0;
    for (int d = 0; d < Dim; d++) {
        product += v1[d] * v2[d];
    }
    return product;
}

/*************************** Specialization ***********************************/

template <typename T>
Vec<2, T>::Vec()
    : x(0)
    , y(0)
{
}

template <typename T>
Vec<2, T>::Vec(T x, T y)
    : x(x)
    , y(y)
{
}

template <typename T>
int Vec<2, T>::Dimension() const
{
    return 2;
}

template <typename T>
Vec<2, T> Vec<2, T>::operator-(Vec<2, T> const& other) const
{
    Vec<2, T> vec;
    vec.x = x - other.x;
    vec.y = y - other.y;
    return vec;
}

template <typename T>
Vec<2, T>& Vec<2, T>::operator+=(Vec<2, T> const& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

template <typename T>
T& Vec<2, T>::operator[](int index)
{
    switch (index) {
    default:
    case 0:
        return x;
    case 1:
        return y;
    }
}

template <typename T>
T const& Vec<2, T>::operator[](int index) const
{
    switch (index) {
    default:
    case 0:
        return x;
    case 1:
        return y;
    }
}

template <typename T>
Vec<3, T>::Vec()
    : x(0)
    , y(0)
    , z(0)
{
}

template <typename T>
Vec<3, T>::Vec(T x, T y, T z)
    : x(x)
    , y(y)
    , z(z)
{
}

template <typename T>
int Vec<3, T>::Dimension() const
{
    return 3;
}

template <typename T>
Vec<3, T> Vec<3, T>::operator-(Vec<3, T> const& other) const
{
    Vec<3, T> vec;
    vec.x = x - other.x;
    vec.y = y - other.y;
    vec.z = z - other.z;
    return vec;
}

template <typename T>
Vec<3, T>& Vec<3, T>::operator+=(Vec<3, T> const& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

template <typename T>
T& Vec<3, T>::operator[](int index)
{
    switch (index) {
    default:
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    }
}

template <typename T>
T const& Vec<3, T>::operator[](int index) const
{
    switch (index) {
    default:
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    }
}
