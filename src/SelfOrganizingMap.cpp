#include "SelfOrganizingMap.hpp"
#include "common/Logger.hpp"

SelfOrganizingMap::SelfOrganizingMap(float initialRate, int maxIterations)
    : m_isDone(false)
    , m_isTraining(false)
    , m_maxIterations(maxIterations)
    , m_initialRate(initialRate)
    , m_worker()
    , m_mut()
    , m_cv()
{
    m_iterations = 0;
    m_rate = initialRate;
}

SelfOrganizingMap::~SelfOrganizingMap()
{
    m_isDone = true;
    m_cv.notify_one();

    if (m_worker.joinable()) {
        m_worker.join();
        Logger::info("The worker joined successfully");
    }
}

void SelfOrganizingMap::Train(std::shared_ptr<Lattice> lattice, InputData& dataset)
{
    m_initialNeighborhood
        = sqrt(static_cast<float>(lattice->m_width * lattice->m_width + lattice->m_height * lattice->m_height)) * 0.5f;
    m_timeConstant = m_maxIterations / logf(m_initialNeighborhood);

    Logger::info("Max iterations: %d, Initial Learning Rate: %f", m_maxIterations, m_initialRate);

    void (SelfOrganizingMap::*TrainInternal)(std::shared_ptr<Lattice>, InputData&) = &SelfOrganizingMap::TrainInternal;
    m_worker = std::thread(TrainInternal, std::ref(*this), lattice, std::ref(dataset));

    Logger::info("Training worker created");
    Logger::info("Training has been paused");
}

void SelfOrganizingMap::TrainInternal(std::shared_ptr<Lattice> lattice, InputData& dataset)
{
    while ((m_iterations <= m_maxIterations) && !m_isDone) {
        std::unique_lock lk(m_mut);
        m_cv.wait(lk, [this] { return m_isTraining || m_isDone; });

        glm::vec3 const input = dataset.GetInput();
        float const progress = static_cast<float>(m_iterations);

        m_neighborhood = m_initialNeighborhood * expf(-progress / m_timeConstant);
        m_rate = m_initialRate * expf(-progress / static_cast<float>(m_maxIterations - m_iterations));

        glm::ivec2 const index = FindBMU(*lattice, input);
        Node& bmu = lattice->m_neurons[index.x + index.y * lattice->m_width];
        UpdateNeighborhood(*lattice, input, bmu, m_neighborhood);

        if (lattice->m_flags & LatticeFlags_CyclicX) {
            for (int y = 0; y < lattice->m_height; y++) {
                lattice->m_neurons[y * lattice->m_width + lattice->m_width - 1]
                    = lattice->m_neurons[y * lattice->m_width + 0];
            }
        }

        if (lattice->m_flags & LatticeFlags_CyclicY) {
            for (int x = 0; x < lattice->m_width; x++) {
                lattice->m_neurons[(lattice->m_height - 1) * lattice->m_width + x]
                    = lattice->m_neurons[0 * lattice->m_width + x];
            }
        }

        ++m_iterations;
    }

    m_isDone = true;
}

glm::ivec2 SelfOrganizingMap::FindBMU(Lattice const& lattice, glm::vec3 const& input) const
{
    glm::ivec2 idx;
    float distMin = std::numeric_limits<float>::max();
    for (int i = 0; i < lattice.m_width; i++) {
        for (int j = 0; j < lattice.m_height; j++) {
            float sum = 0;
            for (int k = 0; k < lattice.m_neurons.front().Dimension(); k++) {
                float const diff = input[k] - lattice.m_neurons[i + j * lattice.m_width][k];
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

void SelfOrganizingMap::UpdateNeighborhood(Lattice& lattice, glm::vec3 input, Node const& bmu, float radius)
{
    int const rad = static_cast<int>(radius);
    int const radSqr = rad * rad;

    int w = lattice.Width() - 1;
    int h = lattice.Height() - 1;
    for (int x = bmu.X() - rad; x <= bmu.X() + rad; x++) {
        int modX = x;
        if (lattice.m_flags & LatticeFlags_CyclicX) {
            modX = ((x % w) + w) % w;
        } else {
            if (x < 0 || x >= lattice.m_width)
                continue;
        }
        for (int y = bmu.Y() - rad; y <= bmu.Y() + rad; y++) {
            int modY = y;
            if (lattice.m_flags & LatticeFlags_CyclicY) {
                modY = ((y % h) + h) % h;
            } else {
                if (y < 0 || y >= lattice.m_height)
                    continue;
            }
            float const dx = bmu.X() - x;
            float const dy = bmu.Y() - y;
            float const distToBmuSqr = dx * dx + dy * dy;
            if (distToBmuSqr < radSqr) {
                Node& node = lattice.m_neurons[modX + modY * lattice.m_width];
                float const influence = expf(-distToBmuSqr / (2.0f * radSqr));
                for (int k = 0; k < node.Dimension(); ++k) {
                    node[k] += m_rate * influence * (input[k] - node[k]);
                }
            }
        }
    }
}

void SelfOrganizingMap::ToggleTraining()
{
    m_isTraining = !m_isTraining;
    m_cv.notify_one();

    if (m_isTraining) {
        Logger::info("Training resumed");
    } else {
        Logger::info("Training paused");
    }
}

bool SelfOrganizingMap::IsTrainingDone() const
{
    return m_isDone;
}

int SelfOrganizingMap::GetIterations() const
{
    return m_iterations;
}
