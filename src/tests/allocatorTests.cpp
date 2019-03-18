#include <gtest/gtest.h>
#include <random>
#include <functional>
#include "../base/baseTypes.hpp"
#include "../base/allocators/ObjectPool.hpp"


class DummyDefaultConstructorClass {
public:
    int a, b, c;
};
TEST(ObjectPool, DefaultConstructor) {
    auto pool = ObjectPool<DummyDefaultConstructorClass>(5);

    auto a = pool.create();
    pool.destroy(a);
}

class DummyConstructorClass {
public:
    DummyConstructorClass(int x, int y, int z): a(x), b(y), c(z) {}
    int a, b, c;

    bool operator==(const DummyConstructorClass& lhs) const {
        return a == lhs.a && b == lhs.b && c == lhs.c;
    }
};
TEST(ObjectPool, Constructor) {
    auto pool = ObjectPool<DummyConstructorClass>(5);
    auto a = pool.create(121, 122, 123);
    ASSERT_EQ(a->a, 121);
    ASSERT_EQ(a->b, 122);
    ASSERT_EQ(a->c, 123);
    pool.destroy(a);
}

TEST(ObjectPool, Validation) {
    auto pool = ObjectPool<DummyConstructorClass>(5);
    auto a = pool.create(1, 2, 3);
    auto b = pool.create(1111, 22, 344);
    auto c = pool.create(1, 2, 3);
    auto d = pool.create(1, 2, 3);
    auto e = pool.create(288, 228, 288);
    pool.destroy(a);
    pool.destroy(c);
    auto f = pool.create(5, 6, 8);
    auto g = pool.create(7, 0, -1);
    pool.destroy(d);
    auto h = pool.create(0, 2, 444);
    pool.destroy(e);
    auto i = pool.create(11, 22, 44);

    ASSERT_EQ(*b, DummyConstructorClass(1111, 22, 344)) << b->a << " " << b->b << " " << b->c << std::endl;
    ASSERT_EQ(*f, DummyConstructorClass(5, 6, 8));
    ASSERT_EQ(*g, DummyConstructorClass(7, 0, -1));
    ASSERT_EQ(*h, DummyConstructorClass(0, 2, 444));
    ASSERT_EQ(*i, DummyConstructorClass(11, 22, 44));
}