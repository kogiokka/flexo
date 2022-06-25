#ifndef NODE_H
#define NODE_H

#include <vector>

class Node
{
    int m_x;
    int m_y;
    std::vector<float> m_weights;

public:
    Node(int x, int y, std::vector<float> initWeights);
    Node(Node const& other);
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

#endif
