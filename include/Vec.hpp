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
    float operator[](int index) const
    {
        return mData[index];
    }

private:
    std::array<T, Dim> mData;
};

#endif
