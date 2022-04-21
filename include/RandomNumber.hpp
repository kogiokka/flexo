#pragma once

#include <array>
#include <cstddef>
#include <random>
#include <vector>

template <typename T>
class RandomNumber
{
protected:
    std::random_device device_;
    std::mt19937_64 engine_;

public:
    RandomNumber();
    virtual ~RandomNumber();
    virtual T scalar() = 0;
    virtual std::vector<T> vector(std::size_t dimension) = 0;
    template <std::size_t S>
    std::array<T, S> vector();
};

template <typename T>
RandomNumber<T>::RandomNumber()
    : device_ {}
    , engine_(device_())
{
}

template <typename T>
RandomNumber<T>::~RandomNumber()
{
}
