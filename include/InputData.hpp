#ifndef INCLUDE_DATA_H
#define INCLUDE_DATA_H

#include <array>
#include <glm/glm.hpp>
#include <vector>

#include "Mesh.hpp"
#include "RandomIntNumber.hpp"

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

template <int InDim>
class InputData
{
public:
    InputData();
    InputData(std::vector<glm::vec3> const& positions);
    InputData(std::vector<Vec<InDim>> const& positions);
    void Insert(std::vector<glm::vec3> const& positions);
    void Insert(std::vector<Vec<InDim>> const& positions);
    std::vector<Vec<InDim>> const& GetData() const;
    Vec<InDim> const& GetInput();
    BoundingBox const& GetBoundingBox() const;

private:
    std::vector<Vec<InDim>> m_pos;
    RandomIntNumber<unsigned int> m_rng;
    BoundingBox m_box;

    void CalculateBoundingBox();
};

#include "InputData.inl"
#endif
