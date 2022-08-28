#ifndef DATASET_H
#define DATASET_H

#include <memory>
#include <vector>

#include <wx/event.h>

#include <glm/glm.hpp>

#include "Attachable.hpp"
#include "RandomIntNumber.hpp"

wxDECLARE_EVENT(EVT_ADD_DATASET, wxCommandEvent);

class Dataset
{
    std::vector<glm::vec3> m_pos;
    RandomIntNumber<unsigned int> m_rng;

public:
    Dataset();
    Dataset(std::vector<glm::vec3> const& positions);
    glm::vec3 const& GetInput();
};

class WatermarkingProject;
using ListOfDataset = std::vector<std::shared_ptr<Dataset>>;

class DatasetList final : public AttachableBase, public ListOfDataset
{
    // privatize emplace_back and push_back method, use Add instead.
    using ListOfDataset::emplace_back;
    using ListOfDataset::push_back;

public:
    static DatasetList& Get(WatermarkingProject& project);
    static DatasetList const& Get(WatermarkingProject const& project);

    explicit DatasetList(WatermarkingProject& project);
    DatasetList(DatasetList const&) = delete;
    DatasetList& operator=(DatasetList const&) = delete;
    void Add(std::vector<glm::vec3> const& positions);
    void SetCurrent(unsigned int index);
    std::shared_ptr<Dataset> GetCurrent() const;

private:
    WatermarkingProject& m_project;
    std::shared_ptr<Dataset> m_curr;
};

#endif
