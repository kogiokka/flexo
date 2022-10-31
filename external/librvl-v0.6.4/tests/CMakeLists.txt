if(NOT BUILD_TESTING)
  return()
endif()

add_executable(unit_tests)

target_sources(unit_tests PRIVATE
  unit_tests.c
  rvl_tests.c
)

if ((CMAKE_C_COMPILER_ID STREQUAL "GNU" AND NOT MINGW) OR
    CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(unit_tests PRIVATE -fsanitize=address)
    target_link_options(unit_tests PRIVATE -fsanitize=address)
endif()

target_link_libraries(unit_tests PRIVATE rvl::rvl)

add_test(NAME "Write Regular Grid" COMMAND unit_tests 1)
add_test(NAME "Read Regular Grid" COMMAND unit_tests 2)
add_test(NAME "Write Rectilinear Grid" COMMAND unit_tests 3)
add_test(NAME "Read Rectilinear Grid" COMMAND unit_tests 4)
add_test(NAME "Partially read RVL file" COMMAND unit_tests 5)
add_test(NAME "Uninitialized RVL" COMMAND unit_tests 6)
add_test(NAME "Input/Output" COMMAND unit_tests 7)
