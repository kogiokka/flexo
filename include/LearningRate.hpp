#ifndef LEARNING_RATE_H
#define LEARNING_RATE_H

struct LearningRate {
    explicit LearningRate(float initialRate = 0.0f, int maxSteps = 0);
    float operator()(int t) const;

    float init;
    int tmax;
};

#endif
