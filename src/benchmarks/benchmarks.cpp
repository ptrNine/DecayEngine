#include <benchmark/benchmark.h>


//#define TEST_OUTPUT

#ifdef TEST_OUTPUT
std::size_t aabbs_count = 64;
#else
std::size_t aabbs_count = 100000;
#endif




#include <array>
#include <vector>
#include <random>
#include <ctime>
#include <iostream>

#include <stdlib.h>
#include <malloc.h>
#include <iomanip>
#include <thread>

// AlignmentAllocator impl
template <typename T, std::size_t N = 16>
class AlignmentAllocator {
public:
    typedef T value_type;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    typedef T * pointer;
    typedef const T * const_pointer;

    typedef T & reference;
    typedef const T & const_reference;

public:
    inline AlignmentAllocator() noexcept = default;
    ~AlignmentAllocator      () noexcept = default;

    template <typename T2>
    explicit AlignmentAllocator(const AlignmentAllocator<T2, N> &) noexcept { }


    inline pointer adress(reference r) {
        return &r;
    }

    inline const_pointer adress(const_reference r) const {
        return &r;
    }

    inline pointer allocate(size_type n) {
        return (pointer)std::aligned_alloc(N, n*sizeof(value_type));
    }

    inline void deallocate(pointer p, size_type) {
        std::free(p);
    }

    inline void construct(pointer p, const value_type & wert) {
        new (p) value_type (wert);
    }

    inline void destroy(pointer p) {
        p->~value_type ();
    }

    inline size_type max_size() const noexcept {
        return size_type (-1) / sizeof (value_type);
    }

    template <typename T2>
    struct rebind {
        typedef AlignmentAllocator<T2, N> other;
    };

    bool operator!=(const AlignmentAllocator<T,N>& other) const  {
        return !(*this == other);
    }

    // Returns true if and only if storage allocated from *this
    // can be deallocated from other, and vice versa.
    // Always returns true for stateless allocators.
    bool operator==(const AlignmentAllocator<T,N>& other) const {
        return true;
    }
};


using Float = float;
using Bool  = int32_t;

struct vec3 {
    explicit vec3(Float ix = 0.f, Float iy = 0.f, Float iz = 0.f):
            x(ix), y(iy), z(iz) {}

    void move(const vec3& vec) {
        x += vec.x;
        y += vec.y;
        z += vec.z;
    }

    Float x, y, z;
};

struct vec4 {
    explicit vec4(Float ix = 0.f, Float iy = 0.f, Float iz = 0.f, Float iw = 0.f):
            x(ix), y(iy), z(iz), w(iw) {}

    vec4(const vec3& vec, Float iw):
            x(vec.x), y(vec.y), z(vec.z), w(iw) {}

    void move(const vec4& vec) {
        x += vec.x;
        y += vec.y;
        z += vec.z;
        w += vec.w;
    }

    Float x, y, z, w;
};



using FrustumT    = std::array<vec4, 6>;
using AabbT       = std::pair<vec4, vec4>;
using AabbV       = std::vector<AabbT, AlignmentAllocator<Float, 32>>;
using CullResultV = std::vector<Bool,  AlignmentAllocator<Bool,  32>>;

using FrustumRef = FrustumT&;
using AabRef     = AabbT&;

using FrustumCref = const FrustumT&;
using AabbCref    = const AabbT&;

// Planes as Ax + By + Cz + D
inline Bool frustum_test(AabbCref aabb, FrustumCref planes) {
    Bool pass = 0;
    for (auto& plane : planes)
        pass |= (std::max(aabb.first.x * plane.x, aabb.second.x * plane.x) +
                 std::max(aabb.first.y * plane.y, aabb.second.y * plane.y) +
                 std::max(aabb.first.z * plane.z, aabb.second.z * plane.z) + plane.w)  <= 0;
    return pass;
}

FrustumT generateFrustum() {
    auto frustum __attribute__((aligned(32))) = FrustumT{
            vec4{ -0.758738f,    -0.99875f,    0.0684196f,    3.01219f   },
            vec4{ 0.761164f,     -0.99875f,    0.0315091f,    -3.71924f  },
            vec4{ 0.0339817f,    -0.931207f,   1.3993f,       1.33221f   },
            vec4{ -0.0315549f,   -1.06629f,    -1.29937f,     -2.03926f  },
            vec4{ 0.00242686f,   -1.9976f,     0.0999336f,    -0.907087f },
            vec4{ -1.18627e-07f, 9.76324e-05f, -4.88386e-06f, 0.200044f  }
    };

    return frustum;
}



auto mt = std::mt19937(clock());
auto generateAABBs(
        const vec3& min,
        const vec3& max,
        const vec3& displacementMin,
        const vec3& displacementMax,
        std::size_t count
        ) -> AabbV
{
    // Distributions for sizes
    auto urd_x = std::uniform_real_distribution<Float>(min.x, max.x);
    auto urd_y = std::uniform_real_distribution<Float>(min.y, max.y);
    auto urd_z = std::uniform_real_distribution<Float>(min.z, max.z);

    // Distributions for positions
    auto urd_d_x = std::uniform_real_distribution<Float>(displacementMin.x, displacementMax.x);
    auto urd_d_y = std::uniform_real_distribution<Float>(displacementMin.y, displacementMax.y);
    auto urd_d_z = std::uniform_real_distribution<Float>(displacementMin.z, displacementMax.z);

    auto res = AabbV();
    res.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        auto one = vec3(urd_x(mt), urd_y(mt), urd_z(mt));
        auto two = vec3(urd_x(mt), urd_y(mt), urd_z(mt));

        auto displacement = vec3(urd_d_x(mt), urd_d_y(mt), urd_d_z(mt));
        one.move(displacement);
        two.move(displacement);

        // sort values
        if (one.x > two.x) std::swap(one.x, two.x);
        if (one.y > two.y) std::swap(one.y, two.y);
        if (one.z > two.z) std::swap(one.z, two.z);

        res.emplace_back(vec4(one, 0.f), vec4(two, 0.f));
    }

    return std::move(res);
}


static void BM_AABBFrustumCulling(benchmark::State& state) {
    auto results = CullResultV(aabbs_count);

    auto frustum = generateFrustum();

    auto aabbs = generateAABBs(vec3(0, 0, 0), vec3(10, 10, 10), vec3(-100, -100, -100), vec3(100, 100, 100), aabbs_count);

    for (auto _ : state)
        for (std::size_t i = 0; i < aabbs_count; ++i)
            results[i] = frustum_test(aabbs[i], frustum);
}
BENCHMARK(BM_AABBFrustumCulling);

#include "../graphics/algorithms/FrustumCulling.hpp"
#include "../graphics/algorithms/frustum_culling_asm.hpp"

static void BM_sse_frustum_culling(benchmark::State& state) {
    auto res  = CullResultV(aabbs_count);
    auto res2 = CullResultV(aabbs_count);

    auto aabbs = generateAABBs(vec3(0, 0, 0), vec3(10, 10, 10), vec3(-100, -100, -100), vec3(100, 100, 100), aabbs_count);

    auto frustum = generateFrustum();

    Float frustum_4x_unpacked[6][4][4] __attribute__((aligned(32)));
    for (auto i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            frustum_4x_unpacked[i][0][j] = frustum[i].x;
            frustum_4x_unpacked[i][1][j] = frustum[i].y;
            frustum_4x_unpacked[i][2][j] = frustum[i].z;
            frustum_4x_unpacked[i][3][j] = frustum[i].w;
        }
    }

    for (auto _ : state) {
        x86_64sv_sse_frustum_culling(
                res.data(),
                reinterpret_cast<Float *>(aabbs.data()),
                &frustum_4x_unpacked[0][0][0],
                aabbs_count);
    }


#ifdef TEST_OUTPUT

    for (auto _ : state)
        for (std::size_t i = 0; i < aabbs_count; ++i)
            res2[i] = frustum_test(aabbs[i], frustum);


    std::cout << "SSE ASM  : ";
    for (auto r : res)
        std::cout << ((r != 0) ? "in " : "out") << " ";
    std::cout << std::endl;

    std::cout << "PLAIN C++: ";
    for (auto r : res2)
        std::cout << ((r != 0) ? "in " : "out") << " ";
    std::cout << std::endl;
    std::cout << std::endl;

#endif

}
BENCHMARK(BM_sse_frustum_culling);



static void BM_avx_frustum_culling(benchmark::State& state) {
    auto res  = CullResultV(aabbs_count);
    auto res2 = CullResultV(aabbs_count);

    auto aabbs = generateAABBs(vec3(0, 0, 0), vec3(10, 10, 10), vec3(-100, -100, -100), vec3(100, 100, 100), aabbs_count);

    auto frustum = generateFrustum();

    Float frustum_8x_unpacked[6][4][8] __attribute__((aligned(32)));
    for (auto i = 0; i < 6; ++i) {
        for (int j = 0; j < 8; ++j) {
            frustum_8x_unpacked[i][0][j] = frustum[i].x;
            frustum_8x_unpacked[i][1][j] = frustum[i].y;
            frustum_8x_unpacked[i][2][j] = frustum[i].z;
            frustum_8x_unpacked[i][3][j] = frustum[i].w;
        }
    }

    for (auto _ : state) {
        x86_64sv_avx_frustum_culling(
                res.data(),
                reinterpret_cast<Float *>(aabbs.data()),
                &frustum_8x_unpacked[0][0][0],
                aabbs_count);
    }

#ifdef TEST_OUTPUT

    for (auto _ : state)
        for (std::size_t i = 0; i < aabbs_count; ++i)
            res2[i] = frustum_test(aabbs[i], frustum);


    std::cout << "AVX ASM  : ";
    for (auto r : res)
        std::cout << ((r != 0) ? "in " : "out") << " ";
    std::cout << std::endl;

    std::cout << "PLAIN C++: ";
    for (auto r : res2)
        std::cout << ((r != 0) ? "in " : "out") << " ";
    std::cout << std::endl;
    std::cout << std::endl;

#endif

}
BENCHMARK(BM_avx_frustum_culling);



void sse_multithread_frustum(int32_t* results, float* aabbs, float* frustum, std::size_t count) {
    std::size_t l0 = 0;
    std::size_t l1 = count >> 1;//2;
    //std::size_t l2 = l1 * 2;
    //std::size_t l3 = l1 * 3;

    auto t0 = std::thread(x86_64sv_sse_frustum_culling, results + l0, aabbs + l0 * 8, frustum, l1);
    auto t1 = std::thread(x86_64sv_sse_frustum_culling, results + l1, aabbs + l1 * 8, frustum, l1);
    //auto t2 = std::thread(x86_64sv_sse_frustum_culling, results + l1, aabbs + l2 * 8, frustum, l1);
    //auto t3 = std::thread(x86_64sv_sse_frustum_culling, results + l1, aabbs + l3 * 8, frustum, l1);

    t0.join();
    t1.join();
    //t2.join();
    //t3.join();
}

static void BM_sse_frustumAABB_multithread(benchmark::State& state) {
    auto res  = CullResultV(aabbs_count);
    auto res2 = CullResultV(aabbs_count);

    auto aabbs = generateAABBs(vec3(0, 0, 0), vec3(10, 10, 10), vec3(-100, -100, -100), vec3(100, 100, 100), aabbs_count);

    auto frustum = generateFrustum();

    Float frustum_4x_unpacked[6][4][4] __attribute__((aligned(32)));
    for (auto i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            frustum_4x_unpacked[i][0][j] = frustum[i].x;
            frustum_4x_unpacked[i][1][j] = frustum[i].y;
            frustum_4x_unpacked[i][2][j] = frustum[i].z;
            frustum_4x_unpacked[i][3][j] = frustum[i].w;
        }
    }

    for (auto _ : state) {
        sse_multithread_frustum(
                res.data(),
                reinterpret_cast<Float *>(aabbs.data()),
                &frustum_4x_unpacked[0][0][0],
                aabbs_count);
    }


#ifdef TEST_OUTPUT

    for (auto _ : state)
        for (std::size_t i = 0; i < aabbs_count; ++i)
            res2[i] = frustum_test(aabbs[i], frustum);


    std::cout << "SSE ASM  : ";
    for (auto r : res)
        std::cout << ((r != 0) ? "in " : "out") << " ";
    std::cout << std::endl;

    std::cout << "PLAIN C++: ";
    for (auto r : res2)
        std::cout << ((r != 0) ? "in " : "out") << " ";
    std::cout << std::endl;
    std::cout << std::endl;

#endif

}
BENCHMARK(BM_sse_frustumAABB_multithread);


void avx_multithread_frustum(int32_t* results, float* aabbs, float* frustum, std::size_t count) {
    std::size_t l0 = 0;
    std::size_t l1 = count >> 1;

    auto t0 = std::thread(x86_64sv_avx_frustum_culling, results + l0, aabbs + l0 * 8, frustum, l1);
    auto t1 = std::thread(x86_64sv_avx_frustum_culling, results + l1, aabbs + l1 * 8, frustum, l1);

    t0.join();
    t1.join();
}

static void BM_avx_multithread_AABBfrustum(benchmark::State& state) {
    auto res = CullResultV(aabbs_count);
    auto res2 = CullResultV(aabbs_count);

    auto aabbs = generateAABBs(vec3(0, 0, 0), vec3(10, 10, 10), vec3(-100, -100, -100), vec3(100, 100, 100),
                               aabbs_count);

    auto frustum = generateFrustum();

    Float frustum_8x_unpacked[6][4][8] __attribute__((aligned(32)));
    for (auto i = 0; i < 6; ++i) {
        for (int j = 0; j < 8; ++j) {
            frustum_8x_unpacked[i][0][j] = frustum[i].x;
            frustum_8x_unpacked[i][1][j] = frustum[i].y;
            frustum_8x_unpacked[i][2][j] = frustum[i].z;
            frustum_8x_unpacked[i][3][j] = frustum[i].w;
        }
    }

    for (auto _ : state) {
        avx_multithread_frustum(
                res.data(),
                reinterpret_cast<Float *>(aabbs.data()),
                &frustum_8x_unpacked[0][0][0],
                aabbs_count);
    }

#ifdef TEST_OUTPUT

    for (auto _ : state)
        for (std::size_t i = 0; i < aabbs_count; ++i)
            res2[i] = frustum_test(aabbs[i], frustum);


    std::cout << "AVX ASM MT:";
    for (auto r : res)
        std::cout << ((r != 0) ? "in " : "out") << " ";
    std::cout << std::endl;

    std::cout << "PLAIN C++: ";
    for (auto r : res2)
        std::cout << ((r != 0) ? "in " : "out") << " ";
    std::cout << std::endl;
    std::cout << std::endl;

#endif

}
BENCHMARK(BM_avx_multithread_AABBfrustum);

BENCHMARK_MAIN();