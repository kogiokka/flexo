set(glm_VERSION 0.9.9)
set(wxWidgets_VERSION 3.2)

find_package(glm ${glm_VERSION} QUIET)
find_package(wxWidgets ${wxWidgets_VERSION} COMPONENTS base gl core aui QUIET)

set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

include(FetchContent)

if (NOT glm_FOUND)
    message(STATUS "Could NOT found glm ${glm_VERSION}, populating the bundled one.")
    include("cmake/FetchGLM.cmake")
endif()

if (wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
else()
    message(STATUS "Could NOT found wxWidgets ${wxWidgets_VERSION}, populating the bundled one.")
    include("cmake/FetchWx.cmake")
    set(wxWidgets_LIBRARIES wx::base wx::core wx::gl wx::aui)
endif()

add_subdirectory("${EXTERNAL_DIR}/librvl-v0.7.0")
include("cmake/BuildGLAD.cmake")
include("cmake/BuildStbImage.cmake")
