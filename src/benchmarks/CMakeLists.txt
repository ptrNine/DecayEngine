# NASM init
enable_language(ASM_NASM)

set(BUILD_ARCH ${CMAKE_SYSTEM_PROCESSOR})
message("Architecture: ${BUILD_ARCH}")

if (BUILD_ARCH STREQUAL "x86_64")
    set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -f elf64")
else()
    message(FATAL_ERROR "Unknown architecture ${BUILD_ARCH}. (Implement me?!)")
endif()


find_package(Threads)


include_directories(${3RD_INCLUDE_DIR})

add_executable(Benchmarks
        benchmarks.cpp
        ../graphics/algorithms/asm/x86_64sv_sse_frustum_culling.asm
        ../graphics/algorithms/asm/x86_64sv_avx_frustum_culling.asm
        )
target_link_libraries(Benchmarks Threads::Threads ${3RD_LIB_DIR}/libbenchmark.a)
add_test(NAME Tests COMMAND Tests)