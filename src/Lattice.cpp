#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>

#include "Lattice.hpp"
#include "common/Logger.hpp"

Lattice::Lattice(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_iterCap(0)
    , m_iterRemained(0)
    , m_initRate(0.0f)
    , m_currRate(0.0f)
    , m_neighborhoodRadius(0.0f)
    , m_isTraining(false)
    , m_isTrainingDone(false)
    , m_isQuit(false)
    , m_flags(LatticeFlags_CyclicNone)
    , m_RNG(-1.0f, 1.0f)
    , m_worker()
    , m_mut()
    , m_cv()
{
    for (int j = 0; j < m_height; ++j) {
        for (int i = 0; i < m_width; ++i) {
            m_neurons.emplace_back(i, j, m_RNG.vector(3));
        }
    }
}

Lattice::~Lattice()
{
    m_isQuit = true;
    m_cv.notify_one();

    if (m_worker.joinable()) {
        m_worker.join();
        Logger::info("The worker joined successfully");
    }
}

void Lattice::SetTrainingParameters(float initialRate, int maxIterations, LatticeFlags flags)
{
    m_initRate = initialRate;
    m_iterCap = maxIterations;
    m_flags = flags;
}

void Lattice::TrainInternal(InputData& dataset)
{
    while (m_iterRemained > 0 && not m_isQuit) {
        std::unique_lock lk(m_mut);
        m_cv.wait(lk, [this] { return this->m_isTraining || this->m_isQuit; });

        glm::vec3 const input = dataset.GetInput();
        float const progress = static_cast<float>(m_iterCap - m_iterRemained);

        m_neighborhoodRadius = m_initRadius * expf(-progress / m_timeConst);
        m_currRate = m_initRate * expf(-progress / m_iterRemained);

        glm::ivec2 const index = FindBMU(input);
        Node& bmu = m_neurons[index.x + index.y * m_width];
        UpdateNeighborhood(input, bmu, m_neighborhoodRadius);

        if (m_flags & LatticeFlags_CyclicX) {
            for (int y = 0; y < m_height; y++) {
                m_neurons[y * m_width + m_width - 1] = m_neurons[y * m_width + 0];
            }
        }

        if (m_flags & LatticeFlags_CyclicY) {
            for (int x = 0; x < m_width; x++) {
                m_neurons[(m_height - 1) * m_width + x] = m_neurons[0 * m_width + x];
            }
        }

        --m_iterRemained;
    }

    m_isTrainingDone = true;
}

void Lattice::Train(InputData& dataset)
{
    m_iterRemained = m_iterCap;
    m_currRate = m_initRate;
    m_initRadius = sqrt(static_cast<float>(m_width * m_width + m_height * m_height)) * 0.5f;
    m_timeConst = m_iterCap / logf(m_initRadius);
    m_isTrainingDone = false;

    Logger::info("Max iterations: %d, Initial Learning Rate: %f", m_iterCap, m_initRate);

    void (Lattice::*TrainInternal)(InputData&) = &Lattice::TrainInternal;
    m_worker = std::thread(TrainInternal, std::ref(*this), std::ref(dataset));

    Logger::info("Training worker created");
    Logger::info("Training has been paused");
}

int Lattice::Width() const
{
    return m_width;
}

int Lattice::Height() const
{
    return m_height;
}

int Lattice::IterationCap() const
{
    return m_iterCap;
}

int Lattice::CurrentIteration() const
{
    return m_iterCap - m_iterRemained;
}

float Lattice::NeighborhoodRadius() const
{
    return m_neighborhoodRadius;
}

std::vector<Node> const& Lattice::Neurons() const
{
    return m_neurons;
}

float Lattice::InitialRate() const
{
    return m_initRate;
}

float Lattice::CurrentRate() const
{
    return m_currRate;
}

void Lattice::ToggleTraining()
{
    m_isTraining = not m_isTraining;
    m_cv.notify_one();

    if (m_isTraining) {
        Logger::info("Training resumed");
    } else {
        Logger::info("Training paused");
    }
}

bool Lattice::IsTrainingDone() const
{
    return m_isTrainingDone;
}

glm::ivec2 Lattice::FindBMU(glm::vec3 const& input) const
{
    glm::ivec2 idx;
    float distMin = std::numeric_limits<float>::max();

    for (int i = 0; i < m_width; i++) {
        for (int j = 0; j < m_height; j++) {
            float sum = 0;
            for (int k = 0; k < m_neurons.front().Dimension(); k++) {
                float const diff = input[k] - m_neurons[i + j * m_width][k];
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

    int w = m_width - 1;
    int h = m_height - 1;
    for (int x = bmu.X() - rad; x <= bmu.X() + rad; x++) {
        int modX = x;
        if (m_flags & LatticeFlags_CyclicX) {
            modX = ((x % w) + w) % w;
        } else {
            if (x < 0 || x >= m_width)
                continue;
        }
        for (int y = bmu.Y() - rad; y <= bmu.Y() + rad; y++) {
            int modY = y;
            if (m_flags & LatticeFlags_CyclicY) {
                modY = ((y % h) + h) % h;
            } else {
                if (y < 0 || y >= m_height)
                    continue;
            }
            float const dx = bmu.X() - x;
            float const dy = bmu.Y() - y;
            float const distToBmuSqr = dx * dx + dy * dy;
            if (distToBmuSqr < radSqr) {
                Node& node = m_neurons[modX + modY * m_width];
                float const influence = expf(-distToBmuSqr / (2.0f * radSqr));
                for (int k = 0; k < node.Dimension(); ++k) {
                    node[k] += m_currRate * influence * (input[k] - node[k]);
                }
            }
        }
    }
}
