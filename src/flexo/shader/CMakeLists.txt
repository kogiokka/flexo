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

get_filename_component(dir "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
get_cmake_property(is_multi_config GENERATOR_IS_MULTI_CONFIG)

set(runtime_output_dir
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<$<BOOL:${is_multi_config}>:/$<CONFIG>>")

add_custom_target(shader ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${runtime_output_dir}/${dir}"
    COMMAND ${CMAKE_COMMAND} -E copy
        ${shader_source}
        "${runtime_output_dir}/${dir}"
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
