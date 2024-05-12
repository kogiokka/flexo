#include <filesystem>

#include "ResourcePath.hpp"

std::string ResourcePath::GetOpenGLShaderFile(std::string const& filename)
{
    return (std::filesystem::current_path().parent_path() / INSTALL_LIBDIR "/flexo/program/opengl" / filename).string();
}


std::string ResourcePath::GetImageFile(std::string const& filename)
{
    return (std::filesystem::current_path().parent_path() / INSTALL_DATADIR "/flexo/images" / filename).string();
}
