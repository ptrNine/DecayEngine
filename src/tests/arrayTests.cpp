#include "gtest/gtest.h"
#include "../base/ftl/array.hpp"
#include <sstream>

TEST(ArrayTests, DECOMPOSITION) {
    ftl::Array array{1, 3, 3, 7};
    auto& [a, b, c, d] = array;
    ASSERT_EQ(a, 1);
    ASSERT_EQ(b, 3);
    ASSERT_EQ(c, 3);
    ASSERT_EQ(d, 7);
    c = 33;
    ASSERT_EQ(33, array.at(2));
}

#define keke(X) 1

TEST(ArrayTests, CONSTEXPR_MAP) {
    keke();
    constexpr ftl::Array a{1, 2, -6};
    constexpr auto b = a.cmap([](int itm) { return itm * 0.5; });
    ASSERT_DOUBLE_EQ(0.5, b[0]);
    ASSERT_DOUBLE_EQ(1.0, b[1]);
    ASSERT_DOUBLE_EQ(-3.0, b[2]);
}

TEST(ArrayTests, CONSTEXPR_FILTER) {
    ftl::Array a{1, 2, 3, 4, 5, 6, 7, 8, 9};
    //constexpr ftl::Array a{1, 2, 3, 4, 5, 6, 7, 8, 9};
    //constexpr auto b = a.cfilter([](int itm) { return itm > 4; });
    //b.print();
}

TEST(ArrayTests, MAP) {
    const ftl::Array a{1, 2, 3, 4, 5, 6, 7};
    auto b = a.map([](const int &itm) { return std::to_string(itm); });
    auto str = std::string();
    for(auto& i : b)
        str += i;
    ASSERT_STREQ("1234567", str.c_str());
}

TEST(ArrayTests, TO_STRING) {
    const ftl::Array a{1, 2, 3, 4, 5};
    ASSERT_STREQ("{ 1, 2, 3, 4, 5 }", a.to_string().c_str());
}

TEST(ArrayTests, PRINT) {
    const ftl::Array a{1, 2, 3, 4, 5};
    const ftl::Array<int, 0> b = {};
    ftl::Array c{ftl::Array<std::string, 2>{"heh", "kek"}, ftl::Array<std::string, 2>{"sas", "ses123"}};

    std::stringstream strm1, strm2, strm3;
    strm1 << a;
    strm2 << b;
    strm3 << c;

    ASSERT_STREQ("{ 1, 2, 3, 4, 5 }", strm1.str().c_str());
    ASSERT_STREQ("{}", strm2.str().c_str());
    ASSERT_STREQ("{ { heh, kek }, { sas, ses123 } }", strm3.str().c_str());
}