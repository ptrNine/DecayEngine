#include "FrustumCulling.hpp"

#include "assert.hpp"

frst_st::FrustumStorage::FrustumStorage() {
    aabbs.reserve(65536);
}

frst_st::FrustumStorage::~FrustumStorage() = default;

SizeT frst_st::FrustumStorage::getID(const AabbT& aabb) {
    if (free_aabbs.empty()) {
        aabbs.emplace_back(aabb);
        return aabbs.size() - 1;
    } else {
        auto id = free_aabbs.front();
        free_aabbs.pop();
        aabbs.at(id) = aabb;
        return id;
    }
}

SizeT frst_st::FrustumStorage::getID() {
    if (free_aabbs.empty()) {
        aabbs.emplace_back();
        return aabbs.size() - 1;
    } else {
        auto id = free_aabbs.front();
        free_aabbs.pop();
        return id;
    }
}

void frst_st::FrustumStorage::removeID(SizeT id) {
    ASSERTF(id < aabbs.size(), "ID >= aabbs size ({}, {})", id, aabbs.size());
    free_aabbs.push(id);
}