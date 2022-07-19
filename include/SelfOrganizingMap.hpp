#ifndef SELF_ORGANIZING_MAP_H
#define SELF_ORGANIZING_MAP_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include <glm/glm.hpp>

#include "Attachable.hpp"
#include "InputData.hpp"
#include "Lattice.hpp"

class WatermarkingProject;

class SelfOrganizingMap : public AttachableBase
{
    bool m_isDone;
    bool m_isTraining;
    int m_maxIterations;
    int m_iterations;
    float m_initialRate;
    float m_rate;
    float m_initialNeighborhood;
    float m_neighborhood;
    float m_timeConstant;

    std::thread m_worker;
    std::mutex m_mut;
    std::condition_variable m_cv;

public:
    static SelfOrganizingMap& Get(WatermarkingProject& project);
    static SelfOrganizingMap const& Get(WatermarkingProject const& project);
    explicit SelfOrganizingMap(WatermarkingProject& project);
    ~SelfOrganizingMap();
    void Initialize(std::shared_ptr<InputData> dataset);
    void ToggleTraining();
    bool IsDone() const;
    bool IsTraining() const;
    void SetMaxIterations(int numIterations);
    void SetLearningRate(float rate);
    void SetNeighborhood(float radius);
    int GetIterations() const;
    float GetNeighborhood() const;

private:
    void StopWorker();
    void Train(Lattice& lattice, std::shared_ptr<InputData> dataset);
    glm::ivec2 FindBMU(Lattice const& lattice, glm::vec3 const& input) const;
    void UpdateNeighborhood(Lattice& lattice, glm::vec3 input, Node const& bmu, float radius);

    WatermarkingProject& m_project;
};

#endif
