add_executable(flexo)

set_target_properties(flexo
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(flexo PRIVATE -Wall -Wextra --pedantic-errors -ggdb)
    target_compile_definitions(flexo PRIVATE $<$<CONFIG:Debug>:_GLIBCXX_DEBUG>)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(flexo PRIVATE -Wall -Wextra --pedantic-errors)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(flexo PRIVATE /wd4996)
    target_link_options(flexo PUBLIC /SUBSYSTEM:WINDOWS)
endif()

target_sources(flexo
PRIVATE
    "main.cpp"
    "ProjectSettings.cpp"
    "Project.cpp"
    "ProjectWindow.cpp"

    "dialog/AddDialog.cpp"

    "pane/ControlsPaneBase.cpp"
    "pane/ControlsGroup.cpp"
    "pane/SceneOutlinerPane.cpp"
    "pane/PropertiesPane.cpp"
    "pane/SelfOrganizingMapPane.cpp"
    "pane/SceneViewportPane.cpp"
    "pane/TextureMappingPane.cpp"

    "object/Object.cpp"
    "object/ObjectList.cpp"
    "object/Plane.cpp"
    "object/Cube.cpp"
    "object/Grid.cpp"
    "object/Sphere.cpp"
    "object/Torus.cpp"
    "object/Guides.cpp"
    "object/SurfaceVoxels.cpp"

    "Camera.cpp"
    "Mesh.cpp"
    "EditableMesh.cpp"
    "TransformStack.cpp"
    "VolumetricModelData.cpp"
    "Voxel.cpp"
    "assetlib/BaseImporter.cpp"
    "assetlib/OBJ/OBJImporter.cpp"
    "assetlib/STL/STLImporter.cpp"
    "util/Logger.c"

    "gfx/Graphics.cpp"
    "gfx/Renderer.cpp"
    "gfx/BindStep.cpp"
    "gfx/DrawTask.cpp"
    "gfx/glwr/IGLWRState.cpp"
    "gfx/glwr/IGLWRBuffer.cpp"
    "gfx/glwr/IGLWRInputLayout.cpp"
    "gfx/glwr/IGLWRVertexShader.cpp"
    "gfx/glwr/IGLWRFragmentShader.cpp"
    "gfx/glwr/IGLWRSampler.cpp"
    "gfx/glwr/IGLWRResource.cpp"
    "gfx/glwr/IGLWRTexture2D.cpp"
    "gfx/glwr/IGLWRRenderBuffer.cpp"
    "gfx/glwr/IGLWRRenderTargetView.cpp"
    "gfx/glwr/IGLWRDepthStencilView.cpp"
    "gfx/glwr/IGLWRShaderResourceView.cpp"
    "gfx/VertexBuffer.cpp"
    "gfx/UniformBlock.cpp"
    "gfx/bindable/Primitive.cpp"
    "gfx/bindable/InputLayout.cpp"
    "gfx/bindable/IndexBuffer.cpp"
    "gfx/bindable/UniformBuffer.cpp"
    "gfx/bindable/VertexBuffer.cpp"
    "gfx/bindable/Texture2D.cpp"
    "gfx/bindable/Sampler.cpp"
    "gfx/bindable/RasterizerState.cpp"
    "gfx/bindable/program/VertexShaderProgram.cpp"
    "gfx/bindable/program/FragmentShaderProgram.cpp"
    "gfx/drawable/DrawableBase.cpp"
    "gfx/drawable/Drawable.cpp"
    "gfx/drawable/IndexedDrawable.cpp"
    "gfx/drawable/InstancedDrawable.cpp"
    "gfx/drawable/SolidDrawable.cpp"
    "gfx/drawable/WireDrawable.cpp"
    "gfx/drawable/TexturedDrawable.cpp"

    "LearningRate.cpp"
    "Neighborhood.cpp"
    "SelfOrganizingMap.cpp"
)

target_include_directories(flexo
PRIVATE
    "include/"
)

target_link_libraries(flexo
PRIVATE
    rvl::rvl
    glad
    stb_image
    glm::glm
    OpenGL::GL
    wx::base
    wx::core
    wx::gl
    wx::aui
    ${CMAKE_DL_LIBS}
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(flexo PRIVATE OpenMP::OpenMP_CXX)
endif()

add_custom_command(TARGET flexo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:flexo>/data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/images"
        "$<TARGET_FILE_DIR:flexo>/images"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shader"
        "$<TARGET_FILE_DIR:flexo>/shader"
    COMMENT "Copying resources to build tree..."
    VERBATIM
)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_custom_command(TARGET flexo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:wx::base>"
                "$<TARGET_FILE:wx::core>"
                "$<TARGET_FILE:wx::gl>"
                "$<TARGET_FILE:wx::aui>"
                "$<TARGET_FILE:rvl::rvl>"

                "$<TARGET_FILE_DIR:flexo>"

        COMMENT "Copying dynamic libraries to the build tree..."
    )
endif()
