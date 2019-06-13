#pragma once

#include <cstdlib>
#include "../baseTypes.hpp"

// AlignedAllocator impl
template <typename T, SizeT N = 16>
class AlignedAllocator {
public:
    using PtrT  = T*;
    using CPtrT = const T*;
    using RefT  = T&;
    using CRefT = const T&;

    using value_type      = T;
    using size_type       = SizeT;
    using difference_type = PtrDiff;
    using pointer         = PtrT;
    using const_pointer   = CPtrT;
    using reference       = RefT;
    using const_reference = CRefT;

public:
    AlignedAllocator () noexcept = default;
    ~AlignedAllocator() noexcept = default;

    template <typename T2>
    explicit AlignedAllocator(const AlignedAllocator<T2, N> &) noexcept {}


    PtrT  adress( RefT r)       { return &r; }
    CPtrT adress(CRefT r) const { return &r; }

    PtrT allocate  (SizeT n)       { return reinterpret_cast<PtrT>(std::aligned_alloc(N, n*sizeof(T))); }
    void deallocate(PtrT p, SizeT) { std::free(p); }

    void construct(PtrT p, CRefT wert) { new (p) T(wert); }
    void destroy  (PtrT p)             { p->~T(); }

    SizeT max_size() const noexcept { return SizeT(-1) / sizeof(T); }

    template <typename T2>
    struct rebind {
        using other = AlignedAllocator<T2, N>;
    };

    bool operator==(const AlignedAllocator<T, N>& other) const {
        return true;
    }

    bool operator!=(const AlignedAllocator<T, N>& other) const  {
        return !(*this == other);
    }
};