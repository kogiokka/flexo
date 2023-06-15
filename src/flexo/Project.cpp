#include "Project.hpp"

FlexoProject::FlexoProject()
    : m_frame {}
{
}

FlexoProject::~FlexoProject()
{
}

void FlexoProject::SetWindow(wxFrame* frame)
{
    m_frame = frame;
}

wxFrame* FlexoProject::GetWindow()
{
    return m_frame;
}
