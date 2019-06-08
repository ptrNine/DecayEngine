#pragma once

extern "C" {
    void x86_64sv_sse_frustum_culling(int32_t* results, float* aabbs, float* frustum, std::size_t count);
    void x86_64sv_avx_frustum_culling(int32_t* results, float* aabbs, float* frustum, std::size_t count);
}