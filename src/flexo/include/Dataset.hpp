#ifndef INCLUDE_DATA_H
#define INCLUDE_DATA_H

#include <glm/glm.hpp>
#include <vector>

#include "Mesh.hpp"
#include "RandomIntNumber.hpp"
#include "Vec.hpp"

template <int InDim>
class Dataset
{
public:
    Dataset();
    Dataset(std::vector<glm::vec3> const& positions);
    Dataset(std::vector<Vec<InDim>> const& positions);
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

#include "Dataset.inl"
#endif
