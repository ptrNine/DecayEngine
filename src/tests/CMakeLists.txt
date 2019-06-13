find_package(Threads)

include_directories(${3RD_INCLUDE_DIR})

add_executable(Tests
        tests.cpp
        arrayTests.cpp
        VectorTests.cpp
        allocatorTests.cpp
        stringTests.cpp
        mathVectorTests.cpp
        formatTest.cpp
        FileTests.cpp
        serializeTests.cpp
        ConfigTests.cpp
        RingTests.cpp)
target_link_libraries(Tests Threads::Threads libgtest.a DeBase)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../bin)

if (WIN32)
    target_link_libraries(Tests lua51)
else()
    target_link_libraries(Tests luajit-5.1)
endif()

add_test(NAME Tests COMMAND Tests)
