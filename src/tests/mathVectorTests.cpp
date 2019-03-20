#include "../base/ftl/vector2.hpp"
#include "../base/ftl/vector3.hpp"
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

TEST(MathVectorTests, Vector2f) {
    using ftl::Vector2f;

    auto vec = Vector2f{2.5, 3.5};
    auto& [x, y] = vec;
    y += 1.5;

    ASSERT_EQ(vec.x(), 2.5);
    ASSERT_EQ(vec.y(), 5);

    ASSERT_EQ(Vector2f(5.5, 5), vec + Vector2f(3, 0));
    ASSERT_EQ(Vector2f(0, 0), vec - Vector2f(2.5, 5));
    ASSERT_EQ(Vector2f(2.5, 2.5), Vector2f(0, 0) + 2.5);
    ASSERT_EQ(Vector2f(1.5, 1.5), Vector2f(4, 4) - 2.5);
    ASSERT_EQ(Vector2f(6, 6), Vector2f(1.5, 1.5) * 4);
    ASSERT_EQ(Vector2f(2.3, 2.3), Vector2f(4.6, 4.6) / 2);

    auto vec2 = Vector2f(4.5, 4.5);

    vec2 += Vector2f(2.5, 4.5);
    ASSERT_EQ(vec2, Vector2f(7, 9));

    vec2 -= Vector2f(4.5, 2.5);
    ASSERT_EQ(vec2, Vector2f(2.5, 6.5));

    vec2 += 4.5;
    ASSERT_EQ(vec2, Vector2f(7, 11));

    vec2 -= 2.5;
    ASSERT_EQ(vec2, Vector2f(4.5, 8.5));

    vec2.y() -= 1;
    vec2 /= 1.5;
    ASSERT_EQ(vec2, Vector2f(3, 5));

    vec2 *= 1.5;
    ASSERT_EQ(vec2, Vector2f(4.5, 7.5));

    auto vec3 = Vector2f(3.5, 2.5);

    vec3 += Vector2f(1, 3);
    ASSERT_EQ(vec3, Vector2f(4.5, 5.5));

    vec3 -= Vector2f(2, 1);
    ASSERT_EQ(vec3, Vector2f(2.5, 4.5));

    vec3.set(0.4, 0.3);
    ASSERT_EQ(vec3.magnitude2(), 0.25);

    ASSERT_FLOAT_EQ(vec3.divProduct(Vector2f(-2, 2)), -0.2);

    ASSERT_FLOAT_EQ(vec3.magnitude(), 0.5);

    vec3.set(6, 8);
    ASSERT_EQ(vec3.makeNormalize(), Vector2f(0.6, 0.8));

    constexpr auto vec4 = Vector2f(4.5, 5.4).rawPerpendicular();
    ASSERT_FLOAT_EQ(Vector2f(4.5, 5.4).divProduct(vec4), 0);
}

TEST(MathVectorTests, Vector2Conversions) {
    auto vec1 = ftl::Vector2T(4.5, 5.5);

    ASSERT_FLOAT_EQ(vec1.x(), 4.5);
    ASSERT_FLOAT_EQ(vec1.y(), 5.5);

    auto vec2 = ftl::Vector2Convert<U8>(vec1);

    ASSERT_EQ(vec2.x(), 4);
    ASSERT_EQ(vec2.y(), 5);
}