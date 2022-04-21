#pragma once

#include "RandomNumber.hpp"

#include <array>
#include <cstdint>
#include <random>
#include <type_traits>
#include <vector>

template <typename T>
class RandomIntNumber : public RandomNumber<T>
{
    std::uniform_int_distribution<T> range_;

public:
    RandomIntNumber(T min, T max);
    virtual T scalar() override;
    virtual std::vector<T> vector(std::size_t dimension) override;
    template <std::size_t S>
    std::array<T, S> vector();
};

template <typename T>
RandomIntNumber<T>::RandomIntNumber(T min, T max)
    : RandomNumber<T>()
    , range_ { min, max }
{
    using namespace std;

    // The type must be IntType
    constexpr bool isInt8 = std::is_same_v<int8_t, T>;
    constexpr bool isInt16 = std::is_same_v<int16_t, T>;
    constexpr bool isInt32 = std::is_same_v<int32_t, T>;
    constexpr bool isInt64 = std::is_same_v<int64_t, T>;
    constexpr bool isUInt8 = std::is_same_v<uint8_t, T>;
    constexpr bool isUInt16 = std::is_same_v<uint16_t, T>;
    constexpr bool isUInt32 = std::is_same_v<uint32_t, T>;
    constexpr bool isUInt64 = std::is_same_v<uint64_t, T>;

    static_assert(isInt8 | isInt16 | isInt32 | isInt64 | isUInt8 | isUInt16 | isUInt32 | isUInt64);
}

template <typename T>
T RandomIntNumber<T>::scalar()
{
    return range_(this->engine_);
}

template <typename T>
std::vector<T> RandomIntNumber<T>::vector(std::size_t dimension)
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
std::array<T, S> RandomIntNumber<T>::vector()
{
    std::array<T, S> vec;
    for (auto i = 0; i < S; ++i) {
        vec[i] = range_(this->engine_);
    }
    return vec;
}
