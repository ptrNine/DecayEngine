#include <gtest/gtest.h>

#include "../base/serialization.hpp"

TEST(Serialization, Numbers) {
    U8  a = 0x24;
    U16 b = 0x2233;
    U32 c = 0x33445566;
    U64 d = 0x1122334455667788ULL;

    S8  as = 0x24;
    S16 bs = 0x2233;
    S32 cs = 0x33445566;
    S64 ds = 0x1122334455667788ULL;

    float  f  = 25.5f;
    double f2 = 25.5f;

    ASSERT_TRUE(srlz::serialize(a).to_string()  == "{ 0x24 }");
    ASSERT_TRUE(srlz::serialize(b).to_string()  == "{ 0x22, 0x33 }");
    ASSERT_TRUE(srlz::serialize(c).to_string()  == "{ 0x33, 0x44, 0x55, 0x66 }");
    ASSERT_TRUE(srlz::serialize(d).to_string()  == "{ 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 }");
    ASSERT_TRUE(srlz::serialize(as).to_string() == "{ 0x24 }");
    ASSERT_TRUE(srlz::serialize(bs).to_string() == "{ 0x22, 0x33 }");
    ASSERT_TRUE(srlz::serialize(cs).to_string() == "{ 0x33, 0x44, 0x55, 0x66 }");
    ASSERT_TRUE(srlz::serialize(ds).to_string() == "{ 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 }");

    ASSERT_TRUE(srlz::serialize(f).to_string()  == "{ 0x41, 0xcc, 0x00, 0x00 }");
    ASSERT_TRUE(srlz::serialize(f2).to_string() == "{ 0x40, 0x39, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00 }");
}

class Dummy {
    U32 a;
    U32 b;
    double f;

public:
    Dummy(U32 ia, U32 ib, double iF): a(ia), b(ib), f(iF) {}

    SERIALIZE_METHOD_CONST_SIZE() {
        return sizeof(Dummy); // 16 bytes
    }

    SERIALIZE_METHOD() {
        SERIALIZE(a);
        SERIALIZE(b);
        SERIALIZE(f);
    }
};

TEST(Serialization, SerializableClass) {
    auto a = Dummy(255, 0x22334455, 25.5);
    ASSERT_TRUE(srlz::serialize(a).to_string() ==
        "{ 0x00, 0x00, 0x00, 0xff, 0x22, 0x33, 0x44, 0x55,"
        " 0x40, 0x39, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00 }");
}

class Dummy2 {
    Dummy d;
    U64 a;

public:
    Dummy2(U64 ia, const Dummy& id): a(ia), d(id) {}

    SERIALIZE_METHOD_CONST_SIZE() {
        return sizeof(Dummy2); // 24 bytes
    }

    SERIALIZE_METHOD() {
        SERIALIZE(d);
        SERIALIZE(a);
    }
};

TEST(Serialization, SerializableInnerClasses) {
    auto a = Dummy(255, 0x22334455, 25.5);
    auto b = Dummy2(0x11001100, a);

    ASSERT_TRUE(srlz::serialize(b).to_string() ==
                "{ 0x00, 0x00, 0x00, 0xff, 0x22, 0x33, 0x44, 0x55,"
                " 0x40, 0x39, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,"
                " 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x11, 0x00 }");
}
