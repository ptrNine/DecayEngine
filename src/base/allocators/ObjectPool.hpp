#ifndef TILEENGINE_TBOBJECTPOOL_HPP
#define TILEENGINE_TBOBJECTPOOL_HPP

#include <cassert>

template<typename Type>
class ObjectPool {
    using SizeT = std::size_t;
    using Byte  = unsigned char;
    using TypePtr = Type*;
    static constexpr SizeT _type_size = sizeof(Type);

public:
    explicit ObjectPool(SizeT size): _size(size) {
        _mem        = reinterpret_cast<TypePtr>(new Byte[_size * _type_size]);
        _freeBlocks = new TypePtr[_size];

        _freeLast = &_freeBlocks[0];
        _freePos  = &_freeBlocks[0];
        _freeEnd  = &_freeBlocks[_size];

        // Initialising array of free memory pointers
        auto memPtr  = &_mem[0];
        auto fragPtr = &_freeBlocks[0];
        auto fragEnd = fragPtr + _size;

        while (fragPtr != fragEnd)
            *fragPtr++ = memPtr++;
    }

    ~ObjectPool() {
        delete [] reinterpret_cast<Byte*>(_mem);
        delete [] _freeBlocks;
    }

    template<typename... Args>
    auto create(Args&&... args) -> TypePtr {
        if (_allocated_blocks != _size)
            return new(getFreeBlock()) Type(args...);
        else
            assert(0);// Todo: assert: can't allocate memory
    }

    auto create() -> TypePtr {
        if (_allocated_blocks != _size)
            return new(getFreeBlock()) Type();
        else
            assert(0);
    }

    void destroy(TypePtr object) {
        addFreeBlock(object);
    }

    inline auto getFreeBlock() -> TypePtr {
        ++_allocated_blocks;
        auto block = *_freePos++;
        if (_freePos == _freeEnd)
            _freePos = &_freeBlocks[0];
        return block;
    }

    inline void addFreeBlock(TypePtr ptr) {
        --_allocated_blocks;
        *_freeLast++ = ptr;
        if (_freeLast == _freeEnd)
            _freeLast = &_freeBlocks[0];
    }

private:
    SizeT _allocated_blocks = 0;
    SizeT _size             = 0;

    TypePtr _mem = nullptr;

    TypePtr* _freeLast   = nullptr;
    TypePtr* _freePos    = nullptr;
    TypePtr* _freeEnd    = nullptr;
    TypePtr* _freeBlocks = nullptr;
};


#endif //TILEENGINE_TBOBJECTPOOL_HPP
