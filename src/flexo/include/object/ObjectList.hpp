#ifndef DRAW_LIST_H
#define DRAW_LIST_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <wx/event.h>

#include "Attachable.hpp"
#include "object/Object.hpp"

class FlexoProject;

class ObjectList : public AttachableBase
{
public:
    static ObjectList& Get(FlexoProject& project);
    static ObjectList const& Get(FlexoProject const& project);
    ObjectList(FlexoProject& project);
    void Add(std::shared_ptr<Object> object);
    void Delete(std::string const& id);

public:
    // Forward the iterator functions
    using iterator = std::vector<std::shared_ptr<Object>>::iterator;
    iterator begin();
    iterator end();
    using const_iterator = std::vector<std::shared_ptr<Object>>::const_iterator;
    const_iterator cbegin() const;
    const_iterator cend() const;

    using size_type = std::vector<std::shared_ptr<Object>>::size_type;
    size_type size() const;

private:
    std::vector<std::shared_ptr<Object>> m_list;
    std::unordered_map<enum ObjectType, unsigned int> m_typeCount;
    FlexoProject& m_project;
};

#endif
