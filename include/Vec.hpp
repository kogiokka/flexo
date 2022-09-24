#ifndef VEC_H
#define VEC_H

#include <array>

template <int Dim>
struct Vec {
    template <typename... T>
    explicit Vec(T... args)
        : mData { args... }
    {
    }
    float operator[](int index) const
    {
        return mData[index];
    }

private:
    std::array<float, Dim> mData;
};

#endif
