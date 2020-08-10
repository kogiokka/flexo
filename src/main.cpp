#include "lattice.hpp"
#include "random_vec3.hpp"
#include "main_window.hpp"
#include "shader.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <filesystem>
#include <iostream>
#include <string>


// void
// importFonts(std::filesystem::path dir_path)
// {
//   namespace fs = std::filesystem;
//
//   if (!fs::exists(dir_path) || !fs::is_directory(dir_path))
//     return;
//
//   ImGuiIO& io = ImGui::GetIO();
//   for (fs::directory_entry entry : fs::recursive_directory_iterator(dir_path)) {
//     if (!entry.is_regular_file())
//       continue;
//     auto const ext = entry.path().extension().string();
//     if (ext != ".ttf" && ext != ".otf")
//       continue;
//     io.Fonts->AddFontFromFileTTF(entry.path().c_str(), 18);
//   }
// }

int
main(int argc, char** argv)
{
  MainWindow app("SOM - Surface fitting", 1200, 800);
  app.show();

  return 0;
}
