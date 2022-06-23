#ifndef RANDOM_REAL_NUMBER_H
#define RANDOM_REAL_NUMBER_H

#include <array>
#include <random>
#include <type_traits>
#include <vector>

#include "RandomNumber.hpp"

template <typename T>
class RandomRealNumber : public RandomNumber<T>
{
    std::uniform_real_distribution<T> range_;

public:
    RandomRealNumber(T min, T max);
    virtual T scalar() override;
    virtual std::vector<T> vector(std::size_t dimension) override;
    template <std::size_t S>
    std::array<T, S> vector();
};

template <typename T>
RandomRealNumber<T>::RandomRealNumber(T min, T max)
    : RandomNumber<T>()
    , range_ { min, max }
{
    // The type must be RealType
    constexpr bool isFloat = std::is_same_v<float, T>;
    constexpr bool isDouble = std::is_same_v<double, T>;
    constexpr bool isLongDouble = std::is_same_v<long double, T>;

    static_assert(isFloat | isDouble | isLongDouble);
}

template <typename T>
T RandomRealNumber<T>::scalar()
{
    return range_(this->engine_);
}

template <typename T>
std::vector<T> RandomRealNumber<T>::vector(std::size_t dimension)
{
    std::vector<T> vec;

    vec.reserve(dimension);
    for (std::size_t i = 0; i < dimension; ++i) {
        vec.push_back(range_(this->engine_));
    }

    return vec;
}

template <typename T>
template <std::size_t S>
std::array<T, S> RandomRealNumber<T>::vector()
{
    std::array<T, S> vec;
    for (auto i = 0; i < S; ++i) {
        vec[i] = range_(this->engine_);
    }
    return vec;
}

#endif
