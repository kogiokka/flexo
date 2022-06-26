#ifndef LATTICE_H
#define LATTICE_H

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "InputData.hpp"
#include "Node.hpp"
#include "RandomRealNumber.hpp"

using LatticeFlags = int;

enum m_LatticeFlags : int {
    LatticeFlags_CyclicNone = 0,
    LatticeFlags_CyclicX,
    LatticeFlags_CyclicY,
};

class Lattice
{
    int m_width;
    int m_height;
    int m_iterCap;
    int m_iterRemained;
    float m_initRate;
    float m_currRate;
    float m_neighborhoodRadius;
    float m_timeConst;
    float m_initRadius;
    bool m_isTraining;
    bool m_isTrainingDone;
    bool m_isQuit;
    LatticeFlags m_flags;
    RandomRealNumber<float> m_RNG;
    std::vector<Node> m_neurons;
    std::thread m_worker;
    std::mutex m_mut;
    std::condition_variable m_cv;

public:
    Lattice(int width, int height);
    ~Lattice();
    int Width() const;
    int Height() const;
    int IterationCap() const;
    int CurrentIteration() const;
    float CurrentRate() const;
    float InitialRate() const;
    float NeighborhoodRadius() const;
    void SetTrainingParameters(float initialRate, int maxIterations, LatticeFlags flags);
    void Train(InputData& dataset);
    std::vector<Node> const& Neurons() const;
    void ToggleTraining();
    bool IsTrainingDone() const;

private:
    void TrainInternal(InputData& dataset);
    glm::ivec2 FindBMU(glm::vec3 const& input) const;
    void UpdateNeighborhood(glm::vec3 input, Node const& bmu, float radius);
};

#endif
