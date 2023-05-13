#ifndef VEC3_EVENT
#define VEC3_EVENT

#include <string>
#include <wx/event.h>

class Vec3Event : public wxEvent
{
public:
    Vec3Event(wxEventType eventType, int winid, float x, float y, float z)
        : wxEvent(winid, eventType)
        , m_x(x)
        , m_y(y)
        , m_z(z)
    {
    }

    float GetX()
    {
        return m_x;
    }

    float GetY()
    {
        return m_y;
    }

    float GetZ()
    {
        return m_z;
    }

    virtual wxEvent* Clone() const
    {
        return new Vec3Event(*this);
    }

private:
    float m_x, m_y, m_z;
};

wxDECLARE_EVENT(EVT_VEC3, Vec3Event);
typedef void (wxEvtHandler::*Vec3EventFunction)(Vec3Event&);
#define Vec3EventHandler(func) (&func)

#endif
