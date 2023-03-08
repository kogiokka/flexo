cmake_minimum_required(VERSION 3.19)

project("LZMA2 Compression")

add_executable(test-lzma2)
target_sources(test-lzma2 PRIVATE "lzma2.c")
target_link_libraries(test-lzma2 PRIVATE rvl-test)

add_test_cwd("LZMA2 compression method" test-lzma2)
