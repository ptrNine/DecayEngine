set(BaseSources
        logs.cpp
        ftl/string.cpp
        configs.cpp
        filesystem.cpp
        files.cpp
        time.cpp
        )

set(BaseHeaders
        configs.hpp
        ftl/array.hpp
        ftl/vector3.hpp
        ftl/ring.hpp
        ftl/vector.hpp
        ftl/string.hpp
        ftl/cp_string.hpp
        ftl/containers_base.hpp
        ftl/function_traits.hpp
        ftl/vector2.hpp
        allocators/ObjectPool.hpp
        allocators/AlignedAllocator.hpp
        logs.hpp
        concepts.hpp
        aton.hpp
        filesystem.hpp
        files.hpp
        flags.hpp
        traits.hpp
        baseTypes.hpp
        math.hpp
        time.hpp
        serialization2.hpp
        assert.hpp
        defines.hpp
        serialization.hpp
        )

add_library(DeBase       SHARED ${BaseSources})
add_library(DeBaseStatic STATIC ${BaseSources})

target_link_libraries(DeBase       xxhash fmt)
target_link_libraries(DeBaseStatic xxhash fmt)