#ifndef SELF_ORGANIZING_MAP_H
#define SELF_ORGANIZING_MAP_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include <glm/glm.hpp>
#include <wx/event.h>

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
    void CreateProcedure(Lattice& lattice, std::shared_ptr<InputData> dataset);

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
    void Train(Lattice& lattice, std::shared_ptr<InputData> dataset);

    /**
     * Find the Best Matching Unit
     *
     * Walk through the nodes and calculate the distance between the input vector and their weight vectors.
     * The node having the smallest distance to the input vector is the BMU.
     *
     * @param lattice Lattice we are training
     * @param input   Input vector
     */
    glm::ivec2 FindBMU(Lattice const& lattice, glm::vec3 const& input) const;

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
    void UpdateNeighborhood(Lattice& lattice, glm::vec3 input, Node<3> const& bmu, float radius);

    WatermarkingProject& m_project;
};

#endif
