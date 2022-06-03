#pragma once

#include <vector>

class Node
{
    int x_;
    int y_;
    std::vector<float> weights_;

public:
    Node(int x, int y, std::vector<float> initWeights);
    int& X();
    int& Y();
    int X() const;
    int Y() const;
    Node& operator=(Node const& other);
    float& operator[](int index);
    float operator[](int index) const;
    int Dimension() const;
    std::vector<float>& Weights();
    std::vector<float> const& Weights() const;
};
