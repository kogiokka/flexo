#ifndef SELF_ORGANIZING_MAP_H
#define SELF_ORGANIZING_MAP_H

#include <condition_variable>
#include <glm/glm.hpp>
#include <mutex>
#include <thread>

#include "InputData.hpp"
#include "Lattice.hpp"

class SelfOrganizingMap
{
    bool m_isDone;
    bool m_isTraining;
    int m_maxIterations;
    int m_iterations;
    float m_initialRate;
    float m_rate;
    float m_initialNeighborhood;
    float m_neighborhood;
    float m_timeConstant;

    std::thread m_worker;
    std::mutex m_mut;
    std::condition_variable m_cv;

public:
    SelfOrganizingMap(float initialRate, int maxIterations);
    ~SelfOrganizingMap();
    void Train(Lattice& lattice, InputData& dataset);
    void ToggleTraining();
    bool IsTrainingDone() const;
    int GetIterations() const;

private:
    void TrainInternal(Lattice& lattice, InputData& dataset);
    glm::ivec2 FindBMU(Lattice const& lattice, glm::vec3 const& input) const;
    void UpdateNeighborhood(Lattice& lattice, glm::vec3 input, Node const& bmu, float radius);
};

#endif
