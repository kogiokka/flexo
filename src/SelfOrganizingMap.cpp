#include "SelfOrganizingMap.hpp"
#include "Lattice.hpp"
#include "Project.hpp"
#include "ProjectSettings.hpp"
#include "common/Logger.hpp"

wxDEFINE_EVENT(EVT_SOM_PROCEDURE_INITIALIZED, wxCommandEvent);

static const WatermarkingProject::AttachedObjects::RegisteredFactory factoryKey { [](WatermarkingProject& project) {
    return std::make_shared<SelfOrganizingMap>(project);
} };

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
}

SelfOrganizingMap::~SelfOrganizingMap()
{
    StopWorker();
}

void SelfOrganizingMap::Initialize(std::shared_ptr<InputData> dataset)
{
    {
        StopWorker();
        m_isDone = false;
        m_isTraining = false;
    }

    int const width = Lattice::Get(m_project).GetWidth();
    int const height = Lattice::Get(m_project).GetHeight();
    auto const& settings = ProjectSettings::Get(m_project);

    m_maxIterations = settings.GetMaxIterations();
    m_initialRate = settings.GetLearningRate();
    m_iterations = 0;
    m_rate = m_initialRate;

    float radius = settings.GetNeighborhood();
    if (radius == 0.0f) {
        radius = 0.5f * sqrt(width * width + height * height);
    }

    m_initialNeighborhood = radius;
    m_neighborhood = radius;

    m_timeConstant = m_maxIterations / logf(m_initialNeighborhood);

    auto& lattice = Lattice::Get(m_project);
    void (SelfOrganizingMap::*Train)(Lattice&, std::shared_ptr<InputData>) = &SelfOrganizingMap::Train;
    m_worker = std::thread(Train, std::ref(*this), std::ref(lattice), dataset);

    wxCommandEvent event(EVT_SOM_PROCEDURE_INITIALIZED);
    m_project.ProcessEvent(event);

    Logger::info("Training worker created");
}

void SelfOrganizingMap::Train(Lattice& lattice, std::shared_ptr<InputData> dataset)
{
    auto& neurons = lattice.GetNeurons();
    int const width = lattice.GetWidth();
    int const height = lattice.GetHeight();

    while (m_iterations < m_maxIterations) {
        std::unique_lock lk(m_mut);
        m_cv.wait(lk, [this] { return m_isTraining || m_isDone; });
        if (m_isDone) {
            break;
        }

        glm::vec3 const input = dataset->GetInput();
        float const progress = static_cast<float>(m_iterations);

        m_neighborhood = m_initialNeighborhood * expf(-progress / m_timeConstant);
        m_rate = m_initialRate * expf(-progress / static_cast<float>(m_maxIterations - m_iterations));

        glm::ivec2 const index = FindBMU(lattice, input);
        Node const& bmu = neurons[index.x + index.y * width];
        UpdateNeighborhood(lattice, input, bmu, m_neighborhood);

        if (lattice.GetFlags() & LatticeFlags_CyclicX) {
            for (int y = 0; y < height; y++) {
                neurons[y * width + width - 1] = neurons[y * width + 0];
            }
        }

        if (lattice.GetFlags() & LatticeFlags_CyclicY) {
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
    for (int i = 0; i < lattice.GetWidth(); i++) {
        for (int j = 0; j < lattice.GetHeight(); j++) {
            float sum = 0;
            for (int k = 0; k < lattice.GetNeurons().front().Dimension(); k++) {
                float const diff = input[k] - lattice.GetNeurons()[i + j * lattice.GetWidth()][k];
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
    auto& neurons = lattice.GetNeurons();
    LatticeFlags const flags = lattice.GetFlags();
    int const width = lattice.GetWidth();
    int const height = lattice.GetHeight();

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

void SelfOrganizingMap::SetMaxIterations(int numIterations)
{
    m_maxIterations = numIterations;
    m_timeConstant = m_maxIterations / logf(m_initialNeighborhood);
}

void SelfOrganizingMap::SetLearningRate(float rate)
{
    m_initialRate = rate;
}

void SelfOrganizingMap::SetNeighborhood(float radius)
{
    m_initialNeighborhood = radius;
    m_neighborhood = radius;
    m_timeConstant = m_maxIterations / logf(m_initialNeighborhood);
}

int SelfOrganizingMap::GetIterations() const
{
    return m_iterations;
}

float SelfOrganizingMap::GetNeighborhood() const
{
    return m_neighborhood;
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
