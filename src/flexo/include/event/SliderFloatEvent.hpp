#ifndef SLIDER_FLOAT_EVENT
#define SLIDER_FLOAT_EVENT

#include <wx/event.h>

class SliderFloatEvent : public wxEvent
{
public:
    SliderFloatEvent(wxEventType eventType, int winid, float value)
        : wxEvent(winid, eventType)
        , m_value(value)
    {
    }
    float GetValue() const
    {
        return m_value;
    }
    virtual wxEvent* Clone() const
    {
        return new SliderFloatEvent(*this);
    }

private:
    float m_value;
};

wxDECLARE_EVENT(EVT_SLIDER_FLOAT, SliderFloatEvent);
typedef void (wxEvtHandler::*SliderFloatEventFunction)(SliderFloatEvent&);
#define SliderFloatEventHandler(func) (&func)

#endif
