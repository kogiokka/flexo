#ifndef SCENE_CONTROLLER_H
#define SCENE_CONTROLLER_H

#include <memory>

#include <wx/event.h>

#include "Attachable.hpp"
#include "object/Object.hpp"

#define ADD_OBJECT_LIST                                                                                                \
    X(EVT_ADD_OBJECT_PLANE, "Plane")                                                                                   \
    X(EVT_ADD_OBJECT_GRID, "Grid")                                                                                     \
    X(EVT_ADD_OBJECT_CUBE, "Cube")                                                                                     \
    X(EVT_ADD_OBJECT_UV_SPHERE, "UV Sphere")                                                                           \
    X(EVT_ADD_OBJECT_TORUS, "Torus")                                                                                   \
    X(EVT_ADD_OBJECT_MAP, "Map (3 to 2)")                                                                              \
    X(EVT_ADD_OBJECT_TEST_MAP, "Test Map")                                                                             \
    X(EVT_ADD_OBJECT_TEST_GRID, "Test Grid")

#define X(evt, name) wxDECLARE_EVENT(evt, wxCommandEvent);
ADD_OBJECT_LIST
#undef X

wxDECLARE_EVENT(EVT_DELETE_OBJECT, wxCommandEvent);

class FlexoProject;

class SceneController : public AttachableBase
{
public:
    static SceneController& Get(FlexoProject& project);
    static SceneController const& Get(FlexoProject const& project);

    SceneController(FlexoProject& project);
    void CreateScene();
    void AcceptObject(std::shared_ptr<Object> object);

private:
    void OnImportModel(wxCommandEvent& event);
    void OnAddPlane(wxCommandEvent& event);
    void OnAddGrid(wxCommandEvent& event);
    void OnAddCube(wxCommandEvent& event);
    void OnAddUVSphere(wxCommandEvent& event);
    void OnAddTorus(wxCommandEvent& event);
    void OnAddMap(wxCommandEvent& event);
    void OnDeleteObject(wxCommandEvent& event);

    void OnAddTestMap(wxCommandEvent& event);
    void OnAddTestGrid(wxCommandEvent& event);

    FlexoProject& m_project;
};

#endif
