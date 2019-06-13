cmake_minimum_required(VERSION 3.10)
project(DecayEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")



# NASM init
enable_language(ASM_NASM)

set(BUILD_ARCH ${CMAKE_SYSTEM_PROCESSOR})
message("Architecture: ${BUILD_ARCH}")

if (BUILD_ARCH STREQUAL "x86_64")
    set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -f elf64")
else()
    message(FATAL_ERROR "Unknown architecture ${BUILD_ARCH}. (Implement me?!)")
endif()



set(3RD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rd/include)
set(3RD_LIB_DIR     ${CMAKE_SOURCE_DIR}/3rd/lib)

include_directories(${3RD_INCLUDE_DIR})
link_directories(DecayEngine ${3RD_LIB_DIR})

set(DECAY_EXECUTABLE
        main.cpp
        src/luabind/LuaContext.cpp
        src/graphics/gui/UiBuilder.cpp
        src/base/logs.cpp
        src/base/configs.cpp
        src/base/time.cpp
        src/base/files.cpp
        src/base/files.hpp
        src/base/ftl/string.cpp
        src/base/filesystem.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../bin)
set(LIBRARY_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/../bin)

add_subdirectory(src)

add_executable(DecayEngine ${DECAY_EXECUTABLE} src/base/filesystem.cpp)
link_directories(DecayEngine ${CMAKE_BINARY_DIR}/../bin)

target_link_libraries(DecayEngine xxhash fmt DeGraphics DeInput)
target_include_directories(DecayEngine
        PRIVATE src/base
        PRIVATE src/graphics
        PRIVATE src/input)

if (WIN32)
    target_link_libraries(DecayEngine lua51)
else()
    target_link_libraries(DecayEngine luajit-5.1)
endif()