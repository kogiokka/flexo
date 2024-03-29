#ifndef RANDOM_INT_NUMBER_H
#define RANDOM_INT_NUMBER_H

#include <array>
#include <cstdint>
#include <random>
#include <type_traits>
#include <vector>

#include "RandomNumber.hpp"

template <typename T>
class RandomIntNumber : public RandomNumber<T>
{
    std::uniform_int_distribution<T> m_range;

public:
    RandomIntNumber();
    RandomIntNumber(T min, T max);
    void setRange(T min, T max);
    virtual T scalar() override;
    virtual std::vector<T> vector(std::size_t dimension) override;
    template <std::size_t S>
    std::array<T, S> vector();
};

template <typename T>
RandomIntNumber<T>::RandomIntNumber()
    : RandomNumber<T>()
    , m_range()
{
}

template <typename T>
RandomIntNumber<T>::RandomIntNumber(T min, T max)
    : RandomNumber<T>()
    , m_range { min, max }
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
void RandomIntNumber<T>::setRange(T min, T max)
{
    m_range = std::uniform_int_distribution<T>(min, max);
}

template <typename T>
T RandomIntNumber<T>::scalar()
{
    return m_range(this->m_engine);
}

template <typename T>
std::vector<T> RandomIntNumber<T>::vector(std::size_t dimension)
{
    std::vector<T> vec;

    vec.reserve(dimension);
    for (std::size_t i = 0; i < dimension; ++i) {
        vec.push_back(m_range(this->m_engine));
    }

    return vec;
}

template <typename T>
template <std::size_t S>
std::array<T, S> RandomIntNumber<T>::vector()
{
    std::array<T, S> vec;
    for (auto i = 0; i < S; ++i) {
        vec[i] = m_range(this->m_engine);
    }
    return vec;
}

#endif
