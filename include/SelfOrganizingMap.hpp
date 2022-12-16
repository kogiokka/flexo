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
#include "LearningRate.hpp"
#include "Map.hpp"
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
     * @param map Map we are training
     * @param dataset Dataset as the input space of SOM
     */
    template <int InDim, int OutDim>
    void CreateProcedure(std::shared_ptr<Map<InDim, OutDim>>& map, std::shared_ptr<Dataset<InDim>> dataset);

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
     * @param map Map we are training
     * @param dataset Dataset as the input space of SOM
     */
    template <int InDim, int OutDim>
    void Train(std::shared_ptr<Map<InDim, OutDim>> map, std::shared_ptr<Dataset<InDim>> dataset);

    /**
     * Find the Best Matching Unit
     *
     * Walk through the nodes and calculate the distance between the input vector and their weight vectors.
     * The node having the smallest distance to the input vector is the BMU.
     *
     * @param map Map we are training
     * @param input   Input vector
     */
    template <int InDim, int OutDim>
    glm::ivec2 FindBMU(Map<InDim, OutDim> const& map, Vec<InDim> const& input) const;

    /**
     * Update the neighborhood of the BMU
     *
     * Walk through the nodes and find the nodes within BMU's neighborhood.
     * The node having the smallest distance to the input vector is the BMU.
     *
     * @param map Map we are training.
     * @param input   Input vector
     * @param bmu     The Best Matching Unit
     * @param radius  Neighborhood radius
     */
    template <int InDim, int OutDim>
    void UpdateNeighborhood(Map<InDim, OutDim>& map, Vec<InDim> input, Node<InDim, OutDim> const& bmu,
                            LearningRate learnRate, Neighborhood neighborhood);

    WatermarkingProject& m_project;
};

template <int InDim, int OutDim>
void SelfOrganizingMap::CreateProcedure(std::shared_ptr<Map<InDim, OutDim>>& map,
                                        std::shared_ptr<Dataset<InDim>> dataset)
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

    void (SelfOrganizingMap::*Train)(std::shared_ptr<Map<InDim, OutDim>>, std::shared_ptr<Dataset<InDim>>)
        = &SelfOrganizingMap::Train;
    m_worker = std::thread(Train, std::ref(*this), map, dataset);

    Logger::info("Training worker created");
}

template <int InDim, int OutDim>
void SelfOrganizingMap::Train(std::shared_ptr<Map<InDim, OutDim>> map, std::shared_ptr<Dataset<InDim>> dataset)
{
    auto& nodes = map->nodes;
    int const width = map->size.x;
    int const height = map->size.y;

    while (m_t < m_tmax) {
        std::unique_lock lk(m_mut);
        m_cv.wait(lk, [this] { return m_isTraining || m_isDone; });
        if (m_isDone) {
            break;
        }

        Vec<InDim> const input = dataset->GetInput();
        glm::ivec2 const index = FindBMU(*map, input);
        auto const& bmu = nodes[index.x + index.y * width];
        UpdateNeighborhood(*map, input, bmu, m_learnRate, m_neighborhood);

        if (map->flags & MapFlags_CyclicX) {
            for (int y = 0; y < height; y++) {
                nodes[y * width + width - 1].weights = nodes[y * width + 0].weights;
            }
        }

        if (map->flags & MapFlags_CyclicY) {
            for (int x = 0; x < width; x++) {
                nodes[(height - 1) * width + x].weights = nodes[0 * width + x].weights;
            }
        }

        ++m_t;
    }

    m_isDone = true;
}

template <int InDim, int OutDim>
glm::ivec2 SelfOrganizingMap::FindBMU(Map<InDim, OutDim> const& map, Vec<InDim> const& input) const
{
    glm::ivec2 idx;
    float distMin = std::numeric_limits<float>::max();
    for (int i = 0; i < map.size.x; i++) {
        for (int j = 0; j < map.size.y; j++) {
            Vec<InDim> const diff = input - map.nodes[i + j * map.size.x].weights;
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
void SelfOrganizingMap::UpdateNeighborhood(Map<InDim, OutDim>& map, Vec<InDim> input, Node<InDim, OutDim> const& bmu,
                                           LearningRate learnRate, Neighborhood neighborhood)
{
    auto& neurons = map.nodes;
    MapFlags const flags = map.flags;
    int const width = map.size.x;
    int const height = map.size.y;

    int const rad = static_cast<int>(neighborhood.radius(m_t));
    int const radSqr = rad * rad;

    int w = width - 1;
    int h = height - 1;
    for (int x = bmu.X() - rad; x <= bmu.X() + rad; x++) {
        int modX = x;
        if (flags & MapFlags_CyclicX) {
            modX = ((x % w) + w) % w;
        } else {
            if (x < 0 || x >= width)
                continue;
        }
        for (int y = bmu.Y() - rad; y <= bmu.Y() + rad; y++) {
            int modY = y;
            if (flags & MapFlags_CyclicY) {
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
