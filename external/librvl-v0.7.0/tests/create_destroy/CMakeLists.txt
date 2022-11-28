cmake_minimum_required(VERSION 3.19)

project("Create/Destroy")

add_executable(create_destroy)
target_sources(create_destroy PRIVATE "create_destroy.c")
target_link_libraries(create_destroy rvl-test)

add_test_cwd("Create/Destroy RVL struct" create_destroy)
