#include <filesystem>
#include <wx/filedlg.h>

#include "Project.hpp"
#include "pane/TextureWidget.hpp"

wxDEFINE_EVENT(EVT_TEXTURE_WIDGET_OPEN_IMAGE, wxCommandEvent);

TextureWidget::TextureWidget(wxWindow* parent, FlexoProject& project)
    : ControlsGroup(parent, "Texture", 2)
    , m_project(project)
{
    m_text = AddReadOnlyText("");
    m_open = AddButton("Open");

    m_open->Bind(wxEVT_BUTTON, &TextureWidget::OnOpenImage, this);
}

void TextureWidget::SetTextureName(std::string name)
{
    m_text->Clear();
    *m_text << name;
}

void TextureWidget::OnOpenImage(wxCommandEvent&)
{
    namespace fs = std::filesystem;
    static wxString defaultDir = "";
    wxFileDialog dialog(this, "Open Image", defaultDir, "", "PNG or JPG Images|*.png;*.jpg",
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    dialog.CenterOnParent();

    wxBusyCursor wait;
    if (dialog.ShowModal() == wxID_CANCEL) {
        defaultDir = fs::path(dialog.GetPath().ToStdString()).parent_path().string();
        return;
    }
    wxString const filepath = dialog.GetPath();
    defaultDir = fs::path(filepath.ToStdString()).parent_path().string();

    m_text->Clear();
    *m_text << fs::path(filepath.ToStdString()).filename().string();

    wxCommandEvent event(EVT_TEXTURE_WIDGET_OPEN_IMAGE);
    event.SetString(filepath);
    m_project.ProcessEvent(event);
}
