#include "Lattice.hpp"
#include "common/Logger.hpp"

#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>

Lattice::Lattice(int width, int height)
    : width_(width)
    , height_(height)
    , iterCap_(0)
    , iterRemained_(0)
    , initRate_(0.0f)
    , currRate_(0.0f)
    , neighborhoodRadius_(0.0f)
    , isTraining_(false)
    , isQuit_(false)
    , flags_(LatticeFlags_CyclicNone)
    , RNG_(-1.0f, 1.0f)
    , worker_()
    , mut_()
    , cv_()
{
    for (int j = 0; j < height_; ++j) {
        for (int i = 0; i < width_; ++i) {
            neurons_.emplace_back(i, j, RNG_.vector(3));
        }
    }
}

Lattice::~Lattice()
{
    isQuit_ = true;
    cv_.notify_one();

    if (worker_.joinable()) {
        worker_.join();
        Logger::info("The worker joined successfully");
    }
}

void Lattice::TrainInternal(InputData& dataset)
{
    while (iterRemained_ > 0 && not isQuit_) {
        std::unique_lock lk(mut_);
        cv_.wait(lk, [this] { return this->isTraining_ || this->isQuit_; });

        glm::vec3 const input = dataset.getInput();
        float const progress = static_cast<float>(iterCap_ - iterRemained_);

        neighborhoodRadius_ = initRadius_ * expf(-progress / timeConst_);
        currRate_ = initRate_ * expf(-progress / iterRemained_);

        glm::ivec2 const index = FindBMU(input);
        Node& bmu = neurons_[index.x + index.y * width_];
        UpdateNeighborhood(input, bmu, neighborhoodRadius_);

        --iterRemained_;
    }
}

void Lattice::Train(InputData& dataset, float rate, int iterations, LatticeFlags flags)
{
    flags_ = flags;
    iterCap_ = iterations;
    iterRemained_ = iterations;
    initRate_ = rate;
    currRate_ = rate;
    initRadius_ = sqrt(static_cast<float>(width_ * width_ + height_ * height_)) * 0.5f;
    timeConst_ = iterations / logf(initRadius_);

    Logger::info("Max iterations: %d, Initial Learning Rate: %f", iterCap_, initRate_);

    void (Lattice::*TrainInternal)(InputData&) = &Lattice::TrainInternal;
    worker_ = std::thread(TrainInternal, std::ref(*this), std::ref(dataset));

    Logger::info("Training worker created");
    Logger::info("Training has been paused");
}

int Lattice::Width() const
{
    return width_;
}

int Lattice::Height() const
{
    return height_;
}

int Lattice::IterationCap() const
{
    return iterCap_;
}

int Lattice::CurrentIteration() const
{
    return iterCap_ - iterRemained_;
}

float Lattice::NeighborhoodRadius() const
{
    return neighborhoodRadius_;
}

std::vector<Node> const& Lattice::Neurons() const
{
    return neurons_;
}

float Lattice::InitialRate() const
{
    return initRate_;
}

float Lattice::CurrentRate() const
{
    return currRate_;
}

void Lattice::ToggleTraining()
{
    isTraining_ = not isTraining_;
    cv_.notify_one();

    if (isTraining_) {
        Logger::info("Training resumed");
    } else {
        Logger::info("Training paused");
    }
}

glm::ivec2 Lattice::FindBMU(glm::vec3 const& input) const
{
    glm::ivec2 idx;
    float distMin = std::numeric_limits<float>::max();

    for (int i = 0; i < width_; i++) {
        for (int j = 0; j < height_; j++) {
            float sum = 0;
            for (int k = 0; k < neurons_.front().Dimension(); k++) {
                float const diff = input[k] - neurons_[i + j * width_][k];
                sum += (diff * diff);
            }
            if (distMin > sum) {
                distMin = sum;
                idx = { i, j };
            }
        }
    }

    return idx;
}

void Lattice::UpdateNeighborhood(glm::vec3 input, Node const& bmu, float radius)
{
    int const rad = static_cast<int>(radius);
    int const radSqr = rad * rad;

    for (int i = bmu.X() - rad; i <= bmu.X() + rad; i++) {
        int x = i;
        if (flags_ & LatticeFlags_CyclicX) {
            x = ((i % width_) + width_) % width_;
        } else {
            if (i < 0 || i >= width_)
                continue;
        }
        for (int j = bmu.Y() - rad; j <= bmu.Y() + rad; j++) {
            int y = j;
            if (flags_ & LatticeFlags_CyclicY) {
                y = ((j % height_) + height_) % height_;
            } else {
                if (j < 0 || j >= height_)
                    continue;
            }
            float const dx = bmu.X() - i;
            float const dy = bmu.Y() - j;
            float const distToBmuSqr = dx * dx + dy * dy;
            if (distToBmuSqr < radSqr) {
                Node& node = neurons_[x + y * width_];
                float const influence = expf(-distToBmuSqr / (2.0f * radSqr));
                for (int i = 0; i < node.Dimension(); ++i) {
                    node[i] += currRate_ * influence * (input[i] - node[i]);
                }
            }
        }
    }
}
