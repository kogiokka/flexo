#pragma once

#include "InputData.hpp"
#include "Node.hpp"
#include "RandomRealNumber.hpp"

#include <array>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

class Lattice
{
    int width_;
    int height_;
    int iterCap_;
    int iterRemained_;
    float initRate_;
    float currRate_;
    float neighborhoodRadius_;
    float timeConst_;
    float initRadius_;
    bool isTraining_;
    bool isQuit_;
    RandomRealNumber<float> RNG_;
    std::vector<Node> neurons_;
    std::thread worker_;
    std::mutex mut_;
    std::condition_variable cv_;

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
    void Train(InputData& dataset, float rate, int iterations);
    std::vector<Node> const& Neurons() const;
    void ToggleTraining();
    void QuitTraining();

private:
    void TrainInternal(InputData& dataset);
};
