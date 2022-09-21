cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project("SOM Demo"
    VERSION 1.0.3
    DESCRIPTION "A Self-Organizing Map Demo"
)

find_package(OpenGL REQUIRED)
find_package(wxWidgets 3.1 COMPONENTS base gl core aui REQUIRED)
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
    "src/ProjectSettings.cpp"
    "src/Project.cpp"
    "src/ProjectWindow.cpp"

    "src/pane/ControlsPaneBase.cpp"
    "src/pane/ControlsGroup.cpp"
    "src/pane/SceneOutlinerPane.cpp"
    "src/pane/SelfOrganizingMapPane.cpp"
    "src/pane/WatermarkingPane.cpp"
    "src/pane/SceneViewportPane.cpp"

    "src/Camera.cpp"
    "src/Lattice.cpp"
    "src/Mesh.cpp"
    "src/Node.cpp"
    "src/Util.cpp"
    "src/World.cpp"
    "src/InputData.cpp"
    "src/VolumeData.cpp"
    "src/common/Logger.cpp"
    "src/assetlib/BaseImporter.cpp"
    "src/assetlib/OBJ/OBJImporter.cpp"
    "src/assetlib/STL/STLImporter.cpp"

    "src/gfx/Graphics.cpp"
    "src/gfx/Renderer.cpp"
    "src/gfx/glwr/IGLWRState.cpp"
    "src/gfx/glwr/IGLWRBuffer.cpp"
    "src/gfx/glwr/IGLWRInputLayout.cpp"
    "src/gfx/glwr/IGLWRVertexShader.cpp"
    "src/gfx/glwr/IGLWRFragmentShader.cpp"
    "src/gfx/glwr/IGLWRSampler.cpp"
    "src/gfx/glwr/IGLWRResource.cpp"
    "src/gfx/glwr/IGLWRTexture2D.cpp"
    "src/gfx/glwr/IGLWRRenderBuffer.cpp"
    "src/gfx/glwr/IGLWRRenderTargetView.cpp"
    "src/gfx/glwr/IGLWRDepthStencilView.cpp"
    "src/gfx/glwr/IGLWRShaderResourceView.cpp"
    "src/gfx/Task.cpp"
    "src/gfx/bindable/Primitive.cpp"
    "src/gfx/bindable/InputLayout.cpp"
    "src/gfx/bindable/IndexBuffer.cpp"
    "src/gfx/bindable/UniformBuffer.cpp"
    "src/gfx/bindable/VertexBuffer.cpp"
    "src/gfx/bindable/Texture2D.cpp"
    "src/gfx/bindable/Sampler.cpp"
    "src/gfx/bindable/TransformUniformBuffer.cpp"
    "src/gfx/bindable/RasterizerState.cpp"
    "src/gfx/bindable/program/VertexShaderProgram.cpp"
    "src/gfx/bindable/program/FragmentShaderProgram.cpp"
    "src/gfx/drawable/DrawableBase.cpp"
    "src/gfx/drawable/Drawable.cpp"
    "src/gfx/drawable/IndexedDrawable.cpp"
    "src/gfx/drawable/InstancedDrawable.cpp"
    "src/gfx/drawable/PolygonalModel.cpp"
    "src/gfx/drawable/VolumetricModel.cpp"
    "src/gfx/drawable/LightSource.cpp"
    "src/gfx/drawable/LatticeVertex.cpp"
    "src/gfx/drawable/LatticeEdge.cpp"
    "src/gfx/drawable/LatticeFace.cpp"

    "src/SelfOrganizingMap.cpp"
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
