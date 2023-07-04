#ifndef SCENE_CONTROLLER_H
#define SCENE_CONTROLLER_H

#include <memory>
#include <string>
#include <vector>

#include <wx/event.h>

#include "Attachable.hpp"
#include "object/Object.hpp"

#define ADD_OBJECT_LIST                                                                                                \
    X(EVT_ADD_OBJECT_PLANE, "Plane")                                                                                   \
    X(EVT_ADD_OBJECT_GRID, "Grid")                                                                                     \
    X(EVT_ADD_OBJECT_CUBE, "Cube")                                                                                     \
    X(EVT_ADD_OBJECT_UV_SPHERE, "UV Sphere")                                                                           \
    X(EVT_ADD_OBJECT_TORUS, "Torus")                                                                                   \
    X(EVT_ADD_OBJECT_MAP, "Map (3 to 2)")

#define X(evt, name) wxDECLARE_EVENT(evt, wxCommandEvent);
ADD_OBJECT_LIST
#undef X

wxDECLARE_EVENT(EVT_DELETE_OBJECT, wxCommandEvent);

class FlexoProject;
class SceneViewportPane;

class SceneController : public AttachableBase
{
public:
    static SceneController& Get(FlexoProject& project);
    static SceneController const& Get(FlexoProject const& project);

    SceneController(FlexoProject& project);
    void CreateScene();
    std::weak_ptr<Object> FindObject(std::string const& id) const;
    void SubmitScene(SceneViewportPane& viewport) const;
    std::vector<std::string> GetAllModelsByID() const;
    std::vector<std::string> GetAllMapsByID() const;

private:
    void OnImportModel(wxCommandEvent& event);
    void OnAddPlane(wxCommandEvent& event);
    void OnAddGrid(wxCommandEvent& event);
    void OnAddCube(wxCommandEvent& event);
    void OnAddUVSphere(wxCommandEvent& event);
    void OnAddTorus(wxCommandEvent& event);
    void OnAddMap(wxCommandEvent& event);
    void OnDeleteObject(wxCommandEvent& event);

    FlexoProject& m_project;
};

#endif
