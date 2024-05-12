set(shader_source
    "LightSource.vert"
    "LightSource.frag"
    "Screen.vert"
    "Screen.frag"
    "SolidDrawable.vert"
    "SolidDrawable.frag"
    "TexturedDrawable.vert"
    "TexturedDrawable.frag"
    "WireDrawable.vert"
    "WireDrawable.frag"
)

get_target_property(flexo_SOURCE_DIR flexo SOURCE_DIR)
cmake_path(RELATIVE_PATH CMAKE_CURRENT_SOURCE_DIR BASE_DIRECTORY "${flexo_SOURCE_DIR}" OUTPUT_VARIABLE dir)

add_custom_target(shader ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${dir}"
    COMMAND ${CMAKE_COMMAND} -E copy
        ${shader_source}
        "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${dir}"
    DEPENDS
        ${shader_source}
    WORKING_DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}"
    SOURCES
        ${shader_source}
    COMMENT "Copying shader programs to build tree..."
    COMMAND_EXPAND_LISTS
    VERBATIM
)

set(flexo_program_opengl_install_libdir "${CMAKE_INSTALL_LIBDIR}/flexo/${dir}")
install(
    FILES ${shader_source}
    DESTINATION "${flexo_program_opengl_install_libdir}"
)
