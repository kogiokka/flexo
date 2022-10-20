add_executable(unit_tests)

target_sources(unit_tests PRIVATE
  unit_tests.c
  rvl_tests.c
)

target_link_libraries(unit_tests PRIVATE rvl::rvl)

add_test(NAME "Write Regular Grid" COMMAND unit_tests 1)
add_test(NAME "Read Regular Grid" COMMAND unit_tests 2)
add_test(NAME "Write Rectilinear Grid" COMMAND unit_tests 3)
add_test(NAME "Read Rectilinear Grid" COMMAND unit_tests 4)
add_test(NAME "Partially read RVL file" COMMAND unit_tests 5)
add_test(NAME "Uninitialized RVL" COMMAND unit_tests 6)
