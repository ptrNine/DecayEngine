#include "../base/ftl/vector2.hpp"
#include <gtest/gtest.h>

TEST(MathVectorTests, Vector2u) {
    using ftl::Vector2u;

    auto vec = Vector2u{2, 3};
    auto& [x, y] = vec;
    y += 1;

    ASSERT_EQ(vec.x(), 2);
    ASSERT_EQ(vec.y(), 4);

    ASSERT_EQ(Vector2u(5, 4), vec + Vector2u(3, 0));
    ASSERT_EQ(Vector2u(0, 0), vec - Vector2u(2, 4));
    ASSERT_EQ(Vector2u(2, 2), Vector2u(0, 0) + 2);
    ASSERT_EQ(Vector2u(2, 2), Vector2u(4, 4) - 2);
    ASSERT_EQ(Vector2u(8, 8), Vector2u(2, 2) * 4);
    ASSERT_EQ(Vector2u(2, 2), Vector2u(4, 4) / 2);

    auto vec2 = Vector2u(4, 4);

    vec2 += Vector2u(2, 4);
    ASSERT_EQ(vec2, Vector2u(6, 8));

    vec2 -= Vector2u(4, 2);
    ASSERT_EQ(vec2, Vector2u(2, 6));

    vec2 += 4;
    ASSERT_EQ(vec2, Vector2u(6, 10));

    vec2 -= 2;
    ASSERT_EQ(vec2, Vector2u(4, 8));

    vec2 /= 4;
    ASSERT_EQ(vec2, Vector2u(1, 2));

    vec2 *= 3;
    ASSERT_EQ(vec2, Vector2u(3, 6));

    auto vec3 = Vector2u(3, 3);

    vec3 += Vector2u(1, 3);
    ASSERT_EQ(vec3, Vector2u(4, 6));

    vec3 -= Vector2u(2, 1);
    ASSERT_EQ(vec3, Vector2u(2, 5));

    ASSERT_EQ(vec3.magnitude2(), 29);

    ASSERT_EQ(vec3.divProduct(Vector2u(2, 2)), 14);

    ASSERT_EQ(vec, Vector2u(2, 4));
    ASSERT_EQ(vec2, Vector2u(3, 6));
    ASSERT_EQ(vec3, Vector2u(2, 5));
}