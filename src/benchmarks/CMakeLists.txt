find_package(Threads)


include_directories(${3RD_INCLUDE_DIR})

add_executable(Benchmarks benchmarks.cpp)

target_include_directories(Benchmarks PRIVATE ../base)

target_link_libraries(Benchmarks Threads::Threads ${3RD_LIB_DIR}/libbenchmark.a DeGraphics)
add_test(NAME Tests COMMAND Tests)