add_library(drawable OBJECT)

set_target_properties(drawable PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(drawable PRIVATE -Wall -Wextra --pedantic-errors -ggdb)
    target_compile_definitions(drawable PRIVATE $<$<CONFIG:Debug>:_GLIBCXX_DEBUG>)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(drawable PRIVATE -Wall -Wextra --pedantic-errors)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(drawable PRIVATE /wd4996)
    target_link_options(drawable PUBLIC /SUBSYSTEM:WINDOWS)
endif()

target_sources(drawable
    PRIVATE
        "Drawable.cpp"
        "IndexedDrawable.cpp"
        "InstancedDrawable.cpp"
        "SolidDrawable.cpp"
        "WireDrawable.cpp"
        "TexturedDrawable.cpp"
        "Overlays.cpp"
)

target_include_directories(drawable PUBLIC include)
target_link_libraries(drawable PRIVATE
    flexo::gfx
    flexo::log
    flexo::util
)
add_library(flexo::drawable ALIAS drawable)
