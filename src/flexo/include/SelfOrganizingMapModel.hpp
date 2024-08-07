#ifndef SELF_ORGANIZING_MAP_MODEL
#define SELF_ORGANIZING_MAP_MODEL

#include <object/Map.hpp>
#include <object/Object.hpp>

template <int InDim, int OutDim>
struct SelfOrganizingMapModel {
    std::weak_ptr<Map<InDim, OutDim>> map;
    std::weak_ptr<Object> object;
    float learningRate;
    unsigned int maxSteps;
    float neighborhood;
};

#endif
