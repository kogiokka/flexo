#include "Vec.hpp"

template <int Dim, typename T>
template <typename... I>
Vec<Dim, T>::Vec(I... args)
    : mData { args... }
{
}

template <int Dim, typename T>
float& Vec<Dim, T>::operator[](int index)
{
    return mData[index];
}

template <int Dim, typename T>
float const& Vec<Dim, T>::operator[](int index) const
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
float operator*(Vec<Dim, T> const& v1, Vec<Dim, T> const& v2)
{
    T product = 0;
    for (int d = 0; d < Dim; d++) {
        product += v1[d] * v2[d];
    }
    return product;
}
