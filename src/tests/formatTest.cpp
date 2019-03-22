#include <gtest/gtest.h>

#include "../base/ftl/vector2.hpp"
#include "../base/ftl/vector3.hpp"
#include "../base/ftl/vector.hpp"
#include "../base/ftl/array.hpp"
#include "../base/ftl/string.hpp"
#include "../base/ftl/cp_string.hpp"
#include "../base/time.hpp"

TEST(Format, Vector2) {
    ftl::Vector2u a {15, 20};
    ftl::Vector2d b {15.5, 13.9};

    ASSERT_EQ(fmt::format("{}", a), "{ 15, 20 }");
    ASSERT_EQ(fmt::format("{}", b), "{ 15.5, 13.9 }");
}

TEST(Format, Vector3) {
    ftl::Vector3u a {15, 20, 12};
    ftl::Vector3d b {15.5, 13.9, 0.00};

    ASSERT_EQ(fmt::format("{}", a), "{ 15, 20, 12 }");
    ASSERT_EQ(fmt::format("{}", b), "{ 15.5, 13.9, 0.0 }");
}

TEST(Format, Vector) {
    ftl::Vector vec {1, 2, 3, 3, 4, 5};
    ASSERT_EQ(fmt::format("{}", vec), "{ 1, 2, 3, 3, 4, 5 }");

    ftl::Vector vec2 {ftl::Vector2u{2, 2}, ftl::Vector2u{3, 5}};
    ASSERT_EQ(fmt::format("{}", vec2), "{ { 2, 2 }, { 3, 5 } }");
}

TEST(Format, Array) {
    ftl::Array arr {1, 2, 3, 3, 4, 5};
    ASSERT_EQ(fmt::format("{}", arr), "{ 1, 2, 3, 3, 4, 5 }");

    ftl::Array arr2 {ftl::Vector2u{2, 2}, ftl::Vector2u{3, 5}};
    ASSERT_EQ(fmt::format("{}", arr2), "{ { 2, 2 }, { 3, 5 } }");
}

TEST(Format, Date) {
    auto date = base::timer().getSystemDateTime();
    date.hour = 1;
    date.min  = 59;
    date.sec  = 10;
    date.ms   = 101;

    ASSERT_EQ(date.to_string("hh:mm:ss:xxx"), "01:59:10:101");
}

TEST(Format, CompileTimeString) {
    auto str = CS("kekkek");
    ASSERT_EQ(fmt::format("{}", str), "kekkek");
}

TEST(Format, String) {
    auto str = ftl::String("sampletext");
    ASSERT_EQ(fmt::format("{}", str), "sampletext");

    str.sprintf("{} and {} and {}", 228, ftl::Vector2u{25, 52}, ftl::Array{1, 2, 3, 4});
    ASSERT_EQ(str, "228 and { 25, 52 } and { 1, 2, 3, 4 }");
}