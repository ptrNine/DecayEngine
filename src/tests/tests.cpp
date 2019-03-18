
#include "../base/ftl/function_traits.hpp"
#include "gtest/gtest.h"
#include "../base/flags.hpp"
#include "../base/math.hpp"
#include "../base/lang_defines.hpp"

TEST(BASE, Flags) {
    using flagger = base::Flags<uint32_t>;

    enum TestEnum {
        ONE   = flagger::def<0>,
        TWO   = flagger::def<1>,
        THREE = flagger::def<2>,
        FOUR  = flagger::def<3>,
        FIVE  = flagger::def<4>
    };

    auto flags = flagger();

    flags.set(ONE | TWO);
    flags.set_if(THREE, true);

    ASSERT_TRUE(flags.test(ONE));
    ASSERT_TRUE(flags.test(TWO));
    ASSERT_TRUE(flags.test(THREE));

    flags.set_if(FOUR, false);

    ASSERT_FALSE(flags.test(FOUR));

    flags.reset();

    ASSERT_EQ(flags.data(), 0);
}


TEST(MATH, fastSqrRoot) {
    auto a = 1/math::fast_inv_sqrt<3>(25.0);
    auto b = 1/math::fast_inv_sqrt<3>(25.f);

    ASSERT_FLOAT_EQ(a, 5.0);
    ASSERT_FLOAT_EQ(b, 5.f);
}


#include "../base/ftl/array.hpp"
#include "../base/ftl/vector.hpp"

#define EVAL_POWER_OF_TWO(x) \
ASSERT_TRUE(math::is_power_of_two_v<array[(x) * 8 + 0]>); \
ASSERT_TRUE(math::is_power_of_two_v<array[(x) * 8 + 1]>); \
ASSERT_TRUE(math::is_power_of_two_v<array[(x) * 8 + 2]>); \
ASSERT_TRUE(math::is_power_of_two_v<array[(x) * 8 + 3]>); \
ASSERT_TRUE(math::is_power_of_two_v<array[(x) * 8 + 4]>); \
ASSERT_TRUE(math::is_power_of_two_v<array[(x) * 8 + 5]>); \
ASSERT_TRUE(math::is_power_of_two_v<array[(x) * 8 + 6]>); \
ASSERT_TRUE(math::is_power_of_two_v<array[(x) * 8 + 7]>)


TEST(MATH, isPowerOfTwo) {
    constexpr auto array = ftl::Array<uint64_t, 64>().cmap([](uint64_t _, std::size_t i) { return 1ULL << i; });

    EVAL_POWER_OF_TWO(0);
    EVAL_POWER_OF_TWO(1);
    EVAL_POWER_OF_TWO(2);
    EVAL_POWER_OF_TWO(3);
    EVAL_POWER_OF_TWO(4);
    EVAL_POWER_OF_TWO(5);
    EVAL_POWER_OF_TWO(6);
    EVAL_POWER_OF_TWO(7);

}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
