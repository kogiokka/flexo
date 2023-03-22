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


    "TransformStack.cpp"
    "VolumetricModelData.cpp"
    "Voxel.cpp"
    "assetlib/BaseImporter.cpp"
    "assetlib/OBJ/OBJImporter.cpp"
    "assetlib/STL/STLImporter.cpp"

    "LearningRate.cpp"
    "Neighborhood.cpp"
    "SelfOrganizingMap.cpp"
)

add_subdirectory(util)
add_subdirectory(log)
add_subdirectory(gfx)
add_subdirectory(drawable)
add_subdirectory(msw)
add_subdirectory(shader)

target_include_directories(flexo PRIVATE include)

target_link_libraries(flexo
PRIVATE
    rvl::rvl
    stb::image
    OpenGL::GL
    wx::base
    wx::core
    wx::gl
    wx::aui
    flexo::gfx
    flexo::drawable
    flexo::util
    flexo::log
    ${CMAKE_DL_LIBS}
)

add_dependencies(flexo shader)

if(OpenMP_CXX_FOUND AND NOT (CMAKE_CXX_COMPILER_ID MATCHES "MSVC"))
    target_link_libraries(flexo PRIVATE OpenMP::OpenMP_CXX)
endif()

add_custom_command(TARGET flexo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:flexo>/data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/images"
        "$<TARGET_FILE_DIR:flexo>/images"
    COMMENT "Copying resources to build tree..."
    VERBATIM
)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(flexo PRIVATE flexo::msw)
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
