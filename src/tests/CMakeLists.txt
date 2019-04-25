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
        ../base/time.cpp
        FileTests.cpp
        ../base/files.cpp
        serializeTests.cpp
        ../base/ftl/string.cpp
        ConfigTests.cpp
        ../base/configs.cpp
        ../base/logs.cpp)
target_link_libraries(Tests Threads::Threads libgtest.a fmt -lstdc++fs xxhash)
add_test(NAME Tests COMMAND Tests)
