#ifndef OBJECT_SELECT_EVENT
#define OBJECT_SELECT_EVENT

#include <string>
#include <wx/event.h>

#include "object/ObjectList.hpp"

class ObjectSelectEvent : public wxEvent
{
public:
    ObjectSelectEvent(wxEventType eventType, int winid, std::string id, ObjectType type)
        : wxEvent(winid, eventType)
        , m_id(id)
        , m_type(type)
    {
    }

    std::string GetId()
    {
        return m_id;
    }

    ObjectType GetType()
    {
        return m_type;
    }

    virtual wxEvent* Clone() const
    {
        return new ObjectSelectEvent(*this);
    }

private:
    std::string m_id;
    ObjectType m_type;
};

wxDECLARE_EVENT(EVT_OBJECT_SELECT, ObjectSelectEvent);
typedef void (wxEvtHandler::*ObjectSelectEventFunction)(ObjectSelectEvent&);
#define ObjectSelectEventHandler(func) (&func)

#endif
