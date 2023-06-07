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
#include "Neighborhood.hpp"
#include "Node.hpp"
#include "SelfOrganizingMapModel.hpp"
#include "Vec.hpp"
#include "log/Logger.h"
#include "object/Map.hpp"

class FlexoProject;

class SelfOrganizingMap
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
    template <int InDim, int OutDim>
    SelfOrganizingMap(SelfOrganizingMapModel<InDim, OutDim>& model, FlexoProject& project);
    ~SelfOrganizingMap();
    SelfOrganizingMap(SelfOrganizingMap const&) = delete;
    SelfOrganizingMap& operator=(SelfOrganizingMap const&) = delete;

    void ToggleTraining();
    bool IsDone() const;
    bool IsTraining() const;

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

    FlexoProject& m_project;
};

template <int InDim, int OutDim>
SelfOrganizingMap::SelfOrganizingMap(SelfOrganizingMapModel<InDim, OutDim>& model, FlexoProject& project)
    : m_isDone(false)
    , m_isTraining(false)
    , m_worker()
    , m_mut()
    , m_cv()
    , m_project(project)
{
    m_t = 0;
    m_tmax = model.maxSteps;
    m_learnRate = LearningRate(model.learningRate, m_tmax);
    m_neighborhood = Neighborhood(NeighborhoodRadius(model.neighborhood, m_tmax));

    if (!model.object) {
        log_fatal("Model is null");
        exit(EXIT_FAILURE);
    }

    auto const& pos = model.object->GetPositions();
    auto dataset = std::make_shared<Dataset<3>>(pos);
    model.object->SetViewFlags(ObjectViewFlag_Solid);
    log_info("Dataset count: %lu", pos.size());

    void (SelfOrganizingMap::*Train)(std::shared_ptr<Map<InDim, OutDim>>, std::shared_ptr<Dataset<InDim>>)
        = &SelfOrganizingMap::Train;
    m_worker = std::thread(Train, std::ref(*this), model.map, dataset);

    log_info("SOM worker created.");
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
