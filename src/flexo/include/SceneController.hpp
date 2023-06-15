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
    X(EVT_ADD_OBJECT_MAP, "Map (3 to 2)")

#define X(evt, name) wxDECLARE_EVENT(evt, wxCommandEvent);
ADD_OBJECT_LIST
#undef X

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
    void OnAddObject(wxCommandEvent& event);
    void OnImportModel(wxCommandEvent& event);

    FlexoProject& m_project;
};

#endif
