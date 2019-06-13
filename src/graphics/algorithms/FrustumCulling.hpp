#pragma once

#include <vector>
#include <queue>

#include <glm/vec4.hpp>

#include "baseTypes.hpp"
#include "defines.hpp"

namespace frst_st {
    class FrustumStorage {
        using AabbT    = std::pair<glm::vec4, glm::vec4>;
        using AabbVecT = std::vector<AabbT>;
        using AabbQueT = std::queue<SizeT>;
        using ResultsT = std::vector<uint32_t>;

    public:
        SizeT getID    (const AabbT& aabb);
        SizeT getID    ();
        void  removeID (SizeT id);

    private:
        AabbVecT aabbs;
        AabbQueT free_aabbs;
        ResultsT results;

    DE_MARK_AS_SINGLETON(FrustumStorage);
    };

} // namespace frst_st


namespace grx {
    class CullingDataProvider {

    };
} // namespace grx