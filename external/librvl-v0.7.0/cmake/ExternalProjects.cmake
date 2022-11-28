#############################################################################
# liblzma
#############################################################################

set(LIBLZMA_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/xz-5.2.7")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build liblzma as a shared library instead of static")
add_subdirectory("${LIBLZMA_SOURCE_DIR}" EXCLUDE_FROM_ALL)
set_target_properties(liblzma PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_options(liblzma PRIVATE $<$<C_COMPILER_ID:GNU>:-fvisibility=hidden>)

if (WIN32)
   target_include_directories(liblzma INTERFACE "$<BUILD_INTERFACE:${LIBLZMA_SOURCE_DIR}/src/liblzma/api>")
endif()


#############################################################################
# lz4
#############################################################################

add_library(lz4 STATIC)

set(LZ4_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/lz4-1.9.4")

set_target_properties(lz4 PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(lz4 PUBLIC "${LZ4_SOURCE_DIR}")
target_sources(lz4
PRIVATE
   "${LZ4_SOURCE_DIR}/lz4.c"
   "${LZ4_SOURCE_DIR}/lz4file.c"
   "${LZ4_SOURCE_DIR}/lz4frame.c"
   "${LZ4_SOURCE_DIR}/lz4hc.c"
   "${LZ4_SOURCE_DIR}/xxhash.c"
)
target_compile_definitions(lz4 PRIVATE LZ4LIB_VISIBILITY=) # empty macro
target_compile_options(lz4 PRIVATE $<$<C_COMPILER_ID:GNU>:-fvisibility=hidden>)
