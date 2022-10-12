add_executable(unit_tests)

target_sources(unit_tests PRIVATE
  unit_tests.c
  rvl_tests.c
)

target_link_libraries(unit_tests PRIVATE rvl::rvl)

add_test(NAME "Write VHDR" COMMAND unit_tests 1)
add_test(NAME "Write DATA" COMMAND unit_tests 2)
add_test(NAME "Write TEXT" COMMAND unit_tests 3)

add_test(NAME "Read VHDR" COMMAND unit_tests 4)
add_test(NAME "Read DATA" COMMAND unit_tests 5)
add_test(NAME "Read TEXT" COMMAND unit_tests 6)
add_test(NAME "Test write" COMMAND unit_tests 7)
add_test(NAME "Test read" COMMAND unit_tests 8)
