#ifndef SELF_ORGANIZING_MAP_H
#define SELF_ORGANIZING_MAP_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include <glm/glm.hpp>
#include <wx/event.h>

#include "Attachable.hpp"
#include "Dataset.hpp"
#include "Lattice.hpp"
#include "LearningRate.hpp"
#include "Neighborhood.hpp"
#include "Node.hpp"
#include "ProjectSettings.hpp"
#include "Vec.hpp"
#include "common/Logger.hpp"

class WatermarkingProject;

class SelfOrganizingMap : public AttachableBase
{
    bool m_isDone;
    bool m_isTraining;
    int m_tmax;
    int m_t;
    LearningRate m_learnRate;
    Neighborhood m_neighborhood;

    std::thread m_worker;
    std::mutex m_mut;
    std::condition_variable m_cv;

public:
    static SelfOrganizingMap& Get(WatermarkingProject& project);
    static SelfOrganizingMap const& Get(WatermarkingProject const& project);
    explicit SelfOrganizingMap(WatermarkingProject& project);
    ~SelfOrganizingMap();
    SelfOrganizingMap(SelfOrganizingMap const&) = delete;
    SelfOrganizingMap& operator=(SelfOrganizingMap const&) = delete;

    /**
     * Setup SOM thraining
     *
     * Set the hyperparameters for SOM and create the worker thread for training.
     *
     * @param lattice Lattice we are training
     * @param dataset Dataset as the input space of SOM
     */
    template <int InDim, int OutDim>
    void CreateProcedure(Lattice<InDim, OutDim>& lattice, std::shared_ptr<Dataset<InDim>> dataset);

    void ToggleTraining();
    bool IsDone() const;
    bool IsTraining() const;
    void StopWorker();
    void OnProjectSettingsChanged(wxCommandEvent& event);

    void SetMaxIterations(int numIterations);
    void SetLearningRate(float rate);
    void SetInitialNeighborhood(float radius);
    int GetIterations() const;
    float GetNeighborhood() const;
    float GetInitialNeighborhood() const;
    float GetLearningRate() const;

private:
    /**
     * Main procedure for SOM training
     *
     * @param lattice Lattice we are training
     * @param dataset Dataset as the input space of SOM
     */
    template <int InDim, int OutDim>
    void Train(Lattice<InDim, OutDim>& lattice, std::shared_ptr<Dataset<InDim>> dataset);

    /**
     * Find the Best Matching Unit
     *
     * Walk through the nodes and calculate the distance between the input vector and their weight vectors.
     * The node having the smallest distance to the input vector is the BMU.
     *
     * @param lattice Lattice we are training
     * @param input   Input vector
     */
    template <int InDim, int OutDim>
    glm::ivec2 FindBMU(Lattice<InDim, OutDim> const& lattice, Vec<InDim> const& input) const;

    /**
     * Update the neighborhood of the BMU
     *
     * Walk through the nodes and find the nodes within BMU's neighborhood.
     * The node having the smallest distance to the input vector is the BMU.
     *
     * @param lattice Lattice we are training.
     * @param input   Input vector
     * @param bmu     The Best Matching Unit
     * @param radius  Neighborhood radius
     */
    template <int InDim, int OutDim>
    void UpdateNeighborhood(Lattice<InDim, OutDim>& lattice, Vec<InDim> input, Node<InDim, OutDim> const& bmu,
                            LearningRate learnRate, Neighborhood neighborhood);

    WatermarkingProject& m_project;
};

template <int InDim, int OutDim>
void SelfOrganizingMap::CreateProcedure(Lattice<InDim, OutDim>& lattice, std::shared_ptr<Dataset<InDim>> dataset)
{
    {
        StopWorker();
        m_isDone = false;
        m_isTraining = false;
    }

    auto const& settings = ProjectSettings::Get(m_project);

    m_t = 0;
    m_tmax = settings.GetMaxIterations();
    m_learnRate = LearningRate(ProjectSettings::Get(m_project).GetLearningRate(), m_tmax);
    m_neighborhood = Neighborhood(NeighborhoodRadius(settings.GetNeighborhood(), m_tmax));

    void (SelfOrganizingMap::*Train)(Lattice<InDim, OutDim>&, std::shared_ptr<Dataset<InDim>>)
        = &SelfOrganizingMap::Train;
    m_worker = std::thread(Train, std::ref(*this), std::ref(lattice), dataset);

    Logger::info("Training worker created");
}

template <int InDim, int OutDim>
void SelfOrganizingMap::Train(Lattice<InDim, OutDim>& lattice, std::shared_ptr<Dataset<InDim>> dataset)
{
    auto& neurons = lattice.mNeurons;
    int const width = lattice.mWidth;
    int const height = lattice.mHeight;

    while (m_t < m_tmax) {
        std::unique_lock lk(m_mut);
        m_cv.wait(lk, [this] { return m_isTraining || m_isDone; });
        if (m_isDone) {
            break;
        }

        Vec<InDim> const input = dataset->GetInput();
        glm::ivec2 const index = FindBMU(lattice, input);
        auto const& bmu = neurons[index.x + index.y * width];
        UpdateNeighborhood(lattice, input, bmu, m_learnRate, m_neighborhood);

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

        ++m_t;
    }

    m_isDone = true;
}

template <int InDim, int OutDim>
glm::ivec2 SelfOrganizingMap::FindBMU(Lattice<InDim, OutDim> const& lattice, Vec<InDim> const& input) const
{
    glm::ivec2 idx;
    float distMin = std::numeric_limits<float>::max();
    for (int i = 0; i < lattice.mWidth; i++) {
        for (int j = 0; j < lattice.mHeight; j++) {
            Vec<InDim> const diff = input - lattice.mNeurons[i + j * lattice.mWidth].weights;
            float const diffLen = diff * diff;
            if (distMin > diffLen) {
                distMin = diffLen;
                idx = { i, j };
            }
        }
    }
    return idx;
}

template <int InDim, int OutDim>
void SelfOrganizingMap::UpdateNeighborhood(Lattice<InDim, OutDim>& lattice, Vec<InDim> input,
                                           Node<InDim, OutDim> const& bmu, LearningRate learnRate,
                                           Neighborhood neighborhood)
{
    auto& neurons = lattice.mNeurons;
    LatticeFlags const flags = lattice.mFlags;
    int const width = lattice.mWidth;
    int const height = lattice.mHeight;

    int const rad = static_cast<int>(neighborhood.radius(m_t));
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
                auto& node = neurons[modX + modY * width];
                Vec<OutDim> const bmuCoord = bmu.coords;
                Vec<OutDim> const nodeCoord(static_cast<float>(x), static_cast<float>(y));
                node.weights += learnRate(m_t) * neighborhood(m_t, bmuCoord, nodeCoord) * (input - node.weights);
            }
        }
    }
}

#endif

