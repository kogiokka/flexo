#ifndef MAP_PROPERTIES_PANE_H
#define MAP_PROPERTIES_PANE_H

#include <memory>

#include "object/Object.hpp"
#include "pane/MeshObjectPropertiesPane.hpp"
#include "pane/TextureWidget.hpp"
#include "pane/TransfromWidget.hpp"
#include "pane/ViewportDisplayWidget.hpp"

class MapPropertiesPane : public MeshObjectPropertiesPane
{
public:
    MapPropertiesPane(wxWindow* parent, FlexoProject& project);
    virtual ~MapPropertiesPane();
    virtual void BindObject(std::shared_ptr<Object> obj) override;

private:
    TextureWidget* m_texture;
};

#endif
