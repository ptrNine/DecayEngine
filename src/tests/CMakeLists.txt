find_package(Threads)

include_directories(${3RD_INCLUDE_DIR})

add_executable(Tests tests.cpp arrayTests.cpp VectorTests.cpp allocatorTests.cpp stringTests.cpp mathVectorTests.cpp)
target_link_libraries(Tests Threads::Threads ${3RD_LIB_DIR}/libgtest.a)
add_test(NAME Tests COMMAND Tests)
