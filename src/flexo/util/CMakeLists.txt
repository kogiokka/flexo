add_library(util OBJECT)

set_target_properties(util
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(util PRIVATE -Wall -Wextra --pedantic-errors -ggdb)
    target_compile_definitions(util PRIVATE $<$<CONFIG:Debug>:_GLIBCXX_DEBUG>)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(util PRIVATE -Wall -Wextra --pedantic-errors)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(util PRIVATE /wd4996)
endif()

target_sources(util
    PRIVATE
        "TransformStack.cpp"
        "Colors.cpp"
)

target_include_directories(util PUBLIC include)
target_link_libraries(util PUBLIC glm::glm)
target_link_libraries(util PRIVATE flexo::log)
add_library(flexo::util ALIAS util)
