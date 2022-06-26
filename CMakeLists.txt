cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project("SOM Demo"
    VERSION 1.0.3
    DESCRIPTION "A Self-Organizing Map Demo"
)

find_package(OpenGL REQUIRED)
find_package(wxWidgets 3.1 COMPONENTS gl core REQUIRED)
find_package(glm REQUIRED)
include(${wxWidgets_USE_FILE})

set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external/")
include("cmake/BuildGLAD.cmake")
include("cmake/BuildStbImage.cmake")
include("cmake/BuildTomlPlusPlus.cmake")

add_executable(som-demo)

set_target_properties(som-demo
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(som-demo PRIVATE -Wall -Wextra --pedantic-errors -ggdb)
    target_compile_definitions(som-demo PRIVATE $<$<CONFIG:Debug>:_GLIBCXX_DEBUG>)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(som-demo PRIVATE -Wall -Wextra --pedantic-errors)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_options(som-demo PUBLIC /SUBSYSTEM:WINDOWS)
endif()

target_sources(som-demo
PRIVATE
    "src/main.cpp"
    "src/MainWindow.cpp"
    "src/MainPanel.cpp"
    "src/OpenGLCanvas.cpp"

    "src/Camera.cpp"
    "src/Renderer.cpp"
    "src/Lattice.cpp"
    "src/Mesh.cpp"
    "src/Node.cpp"
    "src/Util.cpp"
    "src/World.cpp"
    "src/InputData.cpp"
    "src/VolumeData.cpp"
    "src/Image.cpp"
    "src/common/Logger.cpp"
    "src/assetlib/BaseImporter.cpp"
    "src/assetlib/OBJ/OBJImporter.cpp"
    "src/assetlib/STL/STLImporter.cpp"

    "src/Graphics.cpp"
    "src/bindable/Bindable.cpp"
    "src/bindable/Primitive.cpp"
    "src/bindable/VertexLayout.cpp"
    "src/bindable/IndexBuffer.cpp"
    "src/bindable/UniformBuffer.cpp"
    "src/bindable/VertexBuffer.cpp"
    "src/bindable/Texture2D.cpp"
    "src/bindable/Shader.cpp"
    "src/drawable/Drawable.cpp"
    "src/drawable/PolygonalModel.cpp"
    "src/drawable/VolumetricModel.cpp"
    "src/drawable/LightSource.cpp"
    "src/drawable/LatticeVertex.cpp"
    "src/drawable/LatticeEdge.cpp"
    "src/drawable/LatticeFace.cpp"
)

target_include_directories(som-demo
PRIVATE
    "include/"
)

target_link_libraries(som-demo PRIVATE
    glad
    stb_image
    tomlplusplus
    glm::glm
    OpenGL::GL
    ${wxWidgets_LIBRARIES}
    ${CMAKE_DL_LIBS}
)

add_custom_command(TARGET som-demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${CMAKE_SOURCE_DIR}/res"
        "${CMAKE_BINARY_DIR}/res"
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${CMAKE_SOURCE_DIR}/shader"
        "${CMAKE_BINARY_DIR}/shader"
    DEPENDS
        "${CMAKE_SOURCE_DIR}/res"
        "${CMAKE_SOURCE_DIR}/shader"
    COMMENT "Creating symlink from build tree to project resources..."
    VERBATIM
)

include(CPackConfig.cmake)
