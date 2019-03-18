cmake_minimum_required(VERSION 3.13)
project(DecayEngine)

set(CMAKE_CXX_STANDARD 17)

set(3RD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rd/include)
set(3RD_LIB_DIR ${CMAKE_SOURCE_DIR}/3rd/lib)

include_directories(${3RD_INCLUDE_DIR})

set(DECAY_EXECUTABLE
        main.cpp
        src/luabind/LuaContext.cpp
        src/graphics/gui/UiBuilder.cpp src/base/logs.cpp src/base/configs.cpp src/base/configs.hpp src/base/concepts.hpp src/base/ftl/string.hpp)
add_subdirectory(src)

add_executable(DecayEngine ${DECAY_EXECUTABLE})
target_link_libraries(DecayEngine luajit-5.1)