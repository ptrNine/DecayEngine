#pragma once

#include <vector>
#include <queue>

#include <glm/vec4.hpp>

#include "baseTypes.hpp"
#include "allocators/AlignedAllocator.hpp"
#include "ftl/ring.hpp"
#include "defines.hpp"
#include "../Camera.hpp"

namespace frst_st {
    class FrustumStorage {
    public:
        using AabbT     = std::pair<glm::vec4, glm::vec4>;
        using AabbVecT  = std::vector<AabbT, AlignedAllocator<float, 32>>;
        using AabbRingT = ftl::Ring<SizeT>;
        using ResultsT  = std::vector<int32_t, AlignedAllocator<float, 32>>;
        using FrustumT  = grx::Camera::FrustumT;

    public:
        SizeT getID    (const AabbT& aabb);
        SizeT getID    ();
        void  removeID (SizeT id);

        AabbT&  getAabb  (SizeT id) { ASSERT(id < aabbs  .size()); return aabbs  [id]; }
        int32_t getResult(SizeT id) { ASSERT(id < results.size()); return results[id]; }

        void clear() { results.clear(); aabbs.clear(); free_aabbs.clear(); }

        void calculateCulling(const FrustumT& frustum);

    private:
        static void frustum_test_mt(void(*func)(int32_t*, float*, float*, std::size_t),
                int32_t *results, float *aabbs, float *frustum, std::size_t count, std::size_t nprocs);

        void frustum_test(SizeT start, const FrustumT& planes) {
            for (SizeT i = start; i < aabbs.size(); ++i) {
                int32_t pass = 0;
                for (auto &plane : planes)
                    pass |= (std::max(aabbs[i].first.x * plane.x, aabbs[i].second.x * plane.x) +
                             std::max(aabbs[i].first.y * plane.y, aabbs[i].second.y * plane.y) +
                             std::max(aabbs[i].first.z * plane.z, aabbs[i].second.z * plane.z) + plane.w) <= 0;
                results[i] = pass;
            }
        }

    private:
        AabbVecT  aabbs;
        AabbRingT free_aabbs;
        ResultsT  results;

    DE_MARK_AS_SINGLETON(FrustumStorage);
    };

} // namespace frst_st


namespace grx {
    auto& frustum_storage() {
        return frst_st::FrustumStorage::instance();
    }

    class CullingDataProvider {
    public:
        CullingDataProvider() {
            id = frustum_storage().getID();
        }

        CullingDataProvider(const glm::vec4& min, const glm::vec4& max) {
            id = frustum_storage().getID(std::pair(min, max));
        }

        ~CullingDataProvider() {
            frustum_storage().removeID(id);
        }

        auto& aabb  ()       { return frustum_storage().getAabb(id); }
        auto& aabb  () const { return frustum_storage().getAabb(id); }
        auto  result()       { return frustum_storage().getResult(id); }

    private:
        SizeT id;
    };
} // namespace grx