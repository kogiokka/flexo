#include "SelfOrganizingMap.hpp"
#include "Lattice.hpp"
#include "Project.hpp"
#include "ProjectSettings.hpp"
#include "common/Logger.hpp"

// Register factory: SelfOrganizingMap
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<SelfOrganizingMap> {
        return std::make_shared<SelfOrganizingMap>(project);
    }
};

SelfOrganizingMap& SelfOrganizingMap::Get(WatermarkingProject& project)
{
    return project.AttachedObjects::Get<SelfOrganizingMap>(factoryKey);
}

SelfOrganizingMap const& SelfOrganizingMap::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

SelfOrganizingMap::SelfOrganizingMap(WatermarkingProject& project)
    : m_isDone(false)
    , m_isTraining(false)
    , m_worker()
    , m_mut()
    , m_cv()
    , m_project(project)
{
    auto const& settings = ProjectSettings::Get(m_project);

    m_maxIterations = settings.GetMaxIterations();
    m_initialRate = settings.GetLearningRate();
    m_iterations = 0;
    m_neighborhood = 0.0f;
    m_rate = m_initialRate;

    m_project.Bind(EVT_PROJECT_SETTINGS_CHANGED, &SelfOrganizingMap::OnProjectSettingsChanged, this);
}

SelfOrganizingMap::~SelfOrganizingMap()
{
    StopWorker();
}

void SelfOrganizingMap::CreateProcedure(Lattice& lattice, Dataset& dataset)
{
    {
        StopWorker();
        m_isDone = false;
        m_isTraining = false;
    }

    auto const& settings = ProjectSettings::Get(m_project);

    m_maxIterations = settings.GetMaxIterations();
    m_initialRate = settings.GetLearningRate();
    m_initialNeighborhood = settings.GetNeighborhood();
    m_iterations = 0;
    m_rate = m_initialRate;
    m_neighborhood = m_initialNeighborhood;

    void (SelfOrganizingMap::*Train)(Lattice&, Dataset&) = &SelfOrganizingMap::Train;
    m_worker = std::thread(Train, std::ref(*this), std::ref(lattice), std::ref(dataset));

    Logger::info("Training worker created");
}

void SelfOrganizingMap::Train(Lattice& lattice, Dataset& dataset)
{
    auto& neurons = lattice.mNeurons;
    int const width = lattice.mWidth;
    int const height = lattice.mHeight;

    while (m_iterations < m_maxIterations) {
        std::unique_lock lk(m_mut);
        m_cv.wait(lk, [this] { return m_isTraining || m_isDone; });
        if (m_isDone) {
            break;
        }

        glm::vec3 const input = dataset.GetInput();
        float const progress = static_cast<float>(m_iterations);

        float const remains = static_cast<float>(m_maxIterations - m_iterations);
        float const learningRateDecayCoef = 1.0f / remains;
        float const neighborhoodDecayCoef = logf(m_initialNeighborhood) / remains;

        m_neighborhood = m_initialNeighborhood * expf(-progress * neighborhoodDecayCoef);
        m_rate = m_initialRate * expf(-progress * learningRateDecayCoef);

        glm::ivec2 const index = FindBMU(lattice, input);
        Node const& bmu = neurons[index.x + index.y * width];
        UpdateNeighborhood(lattice, input, bmu, m_neighborhood);

        if (lattice.mFlags & LatticeFlags_CyclicX) {
            for (int y = 0; y < height; y++) {
                neurons[y * width + width - 1] = neurons[y * width + 0];
            }
        }

        if (lattice.mFlags & LatticeFlags_CyclicY) {
            for (int x = 0; x < width; x++) {
                neurons[(height - 1) * width + x] = neurons[0 * width + x];
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
    for (int i = 0; i < lattice.mWidth; i++) {
        for (int j = 0; j < lattice.mHeight; j++) {
            float sum = 0;
            for (int k = 0; k < lattice.mNeurons.front().Dimension(); k++) {
                float const diff = input[k] - lattice.mNeurons[i + j * lattice.mWidth][k];
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
    auto& neurons = lattice.mNeurons;
    LatticeFlags const flags = lattice.mFlags;
    int const width = lattice.mWidth;
    int const height = lattice.mHeight;

    int const rad = static_cast<int>(radius);
    int const radSqr = rad * rad;

    int w = width - 1;
    int h = height - 1;
    for (int x = bmu.X() - rad; x <= bmu.X() + rad; x++) {
        int modX = x;
        if (flags & LatticeFlags_CyclicX) {
            modX = ((x % w) + w) % w;
        } else {
            if (x < 0 || x >= width)
                continue;
        }
        for (int y = bmu.Y() - rad; y <= bmu.Y() + rad; y++) {
            int modY = y;
            if (flags & LatticeFlags_CyclicY) {
                modY = ((y % h) + h) % h;
            } else {
                if (y < 0 || y >= height)
                    continue;
            }
            float const dx = bmu.X() - x;
            float const dy = bmu.Y() - y;
            float const distToBmuSqr = dx * dx + dy * dy;
            if (distToBmuSqr < radSqr) {
                Node& node = neurons[modX + modY * width];
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

bool SelfOrganizingMap::IsDone() const
{
    return m_isDone;
}

bool SelfOrganizingMap::IsTraining() const
{
    return m_isTraining;
}

void SelfOrganizingMap::OnProjectSettingsChanged(wxCommandEvent&)
{
    auto const& settings = ProjectSettings::Get(m_project);
    SetMaxIterations(settings.GetMaxIterations());
    SetLearningRate(settings.GetLearningRate());
    SetInitialNeighborhood(settings.GetNeighborhood());
}

void SelfOrganizingMap::SetMaxIterations(int numIterations)
{
    m_maxIterations = numIterations;
}

void SelfOrganizingMap::SetLearningRate(float rate)
{
    m_initialRate = rate;
    m_rate = rate;
}

void SelfOrganizingMap::SetInitialNeighborhood(float radius)
{
    m_initialNeighborhood = radius;
    m_neighborhood = radius;
}

int SelfOrganizingMap::GetIterations() const
{
    return m_iterations;
}

float SelfOrganizingMap::GetNeighborhood() const
{
    return m_neighborhood;
}

float SelfOrganizingMap::GetInitialNeighborhood() const
{
    return m_initialNeighborhood;
}

float SelfOrganizingMap::GetLearningRate() const
{
    return m_rate;
}

void SelfOrganizingMap::StopWorker()
{
    m_isDone = true;
    m_cv.notify_one();

    if (m_worker.joinable()) {
        m_worker.join();
        Logger::info("The worker joined successfully");
    }
}
