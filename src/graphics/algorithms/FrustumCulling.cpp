#include <thread>
#include "FrustumCulling.hpp"

#include "cpu_extension_checker.hpp"
#include "frustum_culling_asm.hpp"
#include "assert.hpp"

frst_st::FrustumStorage::FrustumStorage() {
    aabbs     .reserve(65536);
    results   .reserve(65536);
    free_aabbs.reserve(65536);
}

frst_st::FrustumStorage::~FrustumStorage() = default;

SizeT frst_st::FrustumStorage::getID(const AabbT& aabb) {
    if (free_aabbs.empty()) {
        aabbs.emplace_back(aabb);
        results.emplace_back(0);
        return aabbs.size() - 1;
    } else {
        auto id = free_aabbs.front();
        free_aabbs.pop_back();
        aabbs.at(id) = aabb;
        return id;
    }
}

SizeT frst_st::FrustumStorage::getID() {
    if (free_aabbs.empty()) {
        aabbs.emplace_back();
        results.emplace_back(0);
        return aabbs.size() - 1;
    } else {
        auto id = free_aabbs.front();
        free_aabbs.pop_back();
        return id;
    }
}

void frst_st::FrustumStorage::removeID(SizeT id) {
    ASSERTF(id < aabbs.size(), "ID >= aabbs size ({}, {})", id, aabbs.size());
    free_aabbs.push_back(id);
}


void frst_st::FrustumStorage::frustum_test_mt(
        void(*func)(int32_t*, float*, float*, std::size_t),
        int32_t *results, float *aabbs, float *frustum, std::size_t count, std::size_t nprocs)
{
    std::vector<std::thread> threads;
    for (SizeT i = 0; i < nprocs; ++i)
        threads.emplace_back(
                std::thread(
                        func,
                        results + i * count,
                        aabbs + i * 8 * count, frustum, count));

    for (auto& t : threads)
        t.join();
}


void frst_st::FrustumStorage::calculateCulling(const FrustumT& frustum) {
    SizeT nprocs = std::thread::hardware_concurrency();
    SizeT st_threshold = 512;

    // AVX
    if (base::cpu_extensions_checker().HW_AVX)
    {
        float frustum_8x_unpacked[6][4][8] __attribute__((aligned(32)));

        for (auto i = 0; i < 6; ++i)
            for (int j = 0; j < 8; ++j) {
                frustum_8x_unpacked[i][0][j] = frustum[i].x;
                frustum_8x_unpacked[i][1][j] = frustum[i].y;
                frustum_8x_unpacked[i][2][j] = frustum[i].z;
                frustum_8x_unpacked[i][3][j] = frustum[i].w;
            }

        auto count_per_thread = aabbs.size() / nprocs;

        if (aabbs.size() > st_threshold && count_per_thread > 8) {
            count_per_thread = count_per_thread - (count_per_thread % 8); // must be multiple of 8
            auto remainder_pos = count_per_thread * nprocs;

            frustum_test_mt(x86_64sv_avx_frustum_culling,
                            results.data(),
                            reinterpret_cast<float *>(aabbs.data()),
                            &frustum_8x_unpacked[0][0][0],
                            count_per_thread, nprocs);

            frustum_test(remainder_pos, frustum);
        } else
            frustum_test(0, frustum);

    }
    // SSE2
    else if (base::cpu_extensions_checker().HW_SSE2)
    {
        float frustum_4x_unpacked[6][4][4] __attribute__((aligned(32)));

        for (auto i = 0; i < 6; ++i)
            for (int j = 0; j < 4; ++j) {
                frustum_4x_unpacked[i][0][j] = frustum[i].x;
                frustum_4x_unpacked[i][1][j] = frustum[i].y;
                frustum_4x_unpacked[i][2][j] = frustum[i].z;
                frustum_4x_unpacked[i][3][j] = frustum[i].w;
            }

        auto count_per_thread = aabbs.size() / nprocs;

        if (aabbs.size() > st_threshold && count_per_thread > 4) {
            count_per_thread = count_per_thread - (count_per_thread % 4); // must be multiple of 4
            auto remainder_pos = count_per_thread * nprocs;

            frustum_test_mt(x86_64sv_sse_frustum_culling,
                            results.data(),
                            reinterpret_cast<float *>(aabbs.data()),
                            &frustum_4x_unpacked[0][0][0],
                            count_per_thread, nprocs);

            frustum_test(remainder_pos, frustum);
        } else
            frustum_test(0, frustum);
    }
    else frustum_test(0, frustum);
}