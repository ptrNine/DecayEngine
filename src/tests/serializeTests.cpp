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
    Float32 f  = 25.5f;
    Float64 f2 = 25.5;

    // Serialize
    auto s0 = srlz::serialize(a);
    auto s1 = srlz::serialize(b);
    auto s2 = srlz::serialize(c);
    auto s3 = srlz::serialize(d);
    auto s4 = srlz::serialize(as);
    auto s5 = srlz::serialize(bs);
    auto s6 = srlz::serialize(cs);
    auto s7 = srlz::serialize(ds);
    auto s8 = srlz::serialize(f);
    auto s9 = srlz::serialize(f2);

    // Deserialize
    U8      d0;
    U16     d1;
    U32     d2;
    U64     d3;
    S8      d4;
    S16     d5;
    S32     d6;
    S64     d7;
    Float32 d8;
    Float64 d9;

    auto rc0 = srlz::deserialize(d0, s0.data());
    auto rc1 = srlz::deserialize(d1, s1.data());
    auto rc2 = srlz::deserialize(d2, s2.data());
    auto rc3 = srlz::deserialize(d3, s3.data());
    auto rc4 = srlz::deserialize(d4, s4.data());
    auto rc5 = srlz::deserialize(d5, s5.data());
    auto rc6 = srlz::deserialize(d6, s6.data());
    auto rc7 = srlz::deserialize(d7, s7.data());
    auto rc8 = srlz::deserialize(d8, s8.data());
    auto rc9 = srlz::deserialize(d9, s9.data());

    // Serialize check
    ASSERT_TRUE(s0.to_string() == "{ 0x24 }");
    ASSERT_TRUE(s1.to_string() == "{ 0x22, 0x33 }");
    ASSERT_TRUE(s2.to_string() == "{ 0x33, 0x44, 0x55, 0x66 }");
    ASSERT_TRUE(s3.to_string() == "{ 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 }");
    ASSERT_TRUE(s4.to_string() == "{ 0x24 }");
    ASSERT_TRUE(s5.to_string() == "{ 0x22, 0x33 }");
    ASSERT_TRUE(s6.to_string() == "{ 0x33, 0x44, 0x55, 0x66 }");
    ASSERT_TRUE(s7.to_string() == "{ 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 }");
    ASSERT_TRUE(s8.to_string() == "{ 0x41, 0xcc, 0x00, 0x00 }");
    ASSERT_TRUE(s9.to_string() == "{ 0x40, 0x39, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00 }");

    // Deserialize check
    ASSERT_EQ(d0, 0x24);
    ASSERT_EQ(d1, 0x2233);
    ASSERT_EQ(d2, 0x33445566);
    ASSERT_EQ(d3, 0x1122334455667788);
    ASSERT_EQ(d4, 0x24);
    ASSERT_EQ(d5, 0x2233);
    ASSERT_EQ(d6, 0x33445566);
    ASSERT_EQ(d7, 0x1122334455667788);
    ASSERT_EQ(d8, 25.5f);
    ASSERT_EQ(d9, 25.5);

    // Return code check
    ASSERT_EQ(rc0, 1);
    ASSERT_EQ(rc1, 2);
    ASSERT_EQ(rc2, 4);
    ASSERT_EQ(rc3, 8);
    ASSERT_EQ(rc4, 1);
    ASSERT_EQ(rc5, 2);
    ASSERT_EQ(rc6, 4);
    ASSERT_EQ(rc7, 8);
    ASSERT_EQ(rc8, 4);
    ASSERT_EQ(rc9, 8);
}

struct Dummy {
    U32 a;
    U32 b;
    double f;

    Dummy() = default;
    Dummy(U32 ia, U32 ib, double iF): a(ia), b(ib), f(iF) {}

    SERIALIZE_METHOD_CONST_SIZE() {
        return C_SERIALIZE_GET_SIZE(a) +
               C_SERIALIZE_GET_SIZE(b) +
               C_SERIALIZE_GET_SIZE(f);
    }

    SERIALIZE_METHOD  () { SERIALIZE(a); SERIALIZE(b); SERIALIZE(f); }
    DESERIALIZE_METHOD() { DESERIALIZE(a); DESERIALIZE(b); DESERIALIZE(f); }
};

TEST(Serialization, SerializableClass) {
    auto a = Dummy(255, 0x22334455, 25.5);

    auto s  = a.serialize();
    auto b  = Dummy();
    auto rc = b.deserialize(s.data());

    ASSERT_TRUE(s.to_string() ==
        "{ 0x00, 0x00, 0x00, 0xff, 0x22, 0x33, 0x44, 0x55,"
         " 0x40, 0x39, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00 }");

    ASSERT_EQ(b.a, 255);
    ASSERT_EQ(b.b, 0x22334455);
    ASSERT_EQ(b.f, 25.5);

    ASSERT_EQ(16, rc);
}

struct Dummy2 {
    Dummy d;
    U64 a;

    Dummy2() = default;
    Dummy2(U64 ia, const Dummy& id): a(ia), d(id) {}

    SERIALIZE_METHOD_CONST_SIZE() {
        return C_SERIALIZE_GET_SIZE(d) +
               C_SERIALIZE_GET_SIZE(a);
    }

    SERIALIZE_METHOD() {
        SERIALIZE(d);
        SERIALIZE(a);
    }

    DESERIALIZE_METHOD() {
        DESERIALIZE(d);
        DESERIALIZE(a);
    }
};

TEST(Serialization, SerializableInnerClasses) {
    auto a = Dummy(255, 0x22334455, 25.5);
    auto b = Dummy2(0x11001100, a);

    auto s  = b.serialize();
    auto c  = Dummy2();
    auto rc = c.deserialize(s.data());

    ASSERT_TRUE(s.to_string() ==
                "{ 0x00, 0x00, 0x00, 0xff, 0x22, 0x33, 0x44, 0x55,"
                " 0x40, 0x39, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,"
                " 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x11, 0x00 }");

    ASSERT_EQ(c.a, 0x11001100);
    ASSERT_EQ(c.d.a, 255);
    ASSERT_EQ(c.d.b, 0x22334455);
    ASSERT_EQ(c.d.f, 25.5);

    ASSERT_EQ(rc, 24);
}

TEST(Serialization, U8Ptr) {
    U8 a[6] = {1, 2, 3, 4, 5, 6};
    U8 b[6];

    auto s  = srlz::serialize(a, 6);
    auto rc = srlz::deserialize(b, 6, s.data());

    ASSERT_TRUE(s.to_string() == "{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 }");
    for (SizeT i = 0; i < 6; ++i)
        ASSERT_EQ(a[i], b[i]);
    ASSERT_EQ(rc, 6);
}

TEST(Serialization, U32Ptr) {
    U32 a[3] = {1, 0xFF00FF00, 0x11002233};
    U32 b[3];

    auto s  = srlz::serialize(a, 3);
    auto rc = srlz::deserialize(b, 3, s.data());

    ASSERT_TRUE(srlz::serialize(a, 3).to_string() ==
                "{ 0x00, 0x00, 0x00, 0x01, 0xff, 0x00, 0xff, 0x00, 0x11, 0x00, 0x22, 0x33 }");
    for (SizeT i = 0; i < 3; ++i)
        ASSERT_EQ(a[i], b[i]);
    ASSERT_EQ(rc, 12);
}

struct ClassPtrT {
    U32 a;
    U32 b;
    ClassPtrT() = default;
    ClassPtrT(U32 ia, U32 ib): a(ia), b(ib) {}
    SERIALIZE_METHOD_CONST_SIZE() { return C_SERIALIZE_GET_SIZE(a) + C_SERIALIZE_GET_SIZE(b); }
    SERIALIZE_METHOD  () { SERIALIZE(a); SERIALIZE(b); }
    DESERIALIZE_METHOD() { DESERIALIZE(a); DESERIALIZE(b); }
};

TEST(Serialization, ClassPtrStatic) {
    ClassPtrT a[3] {{1, 2}, {3, 4}, {5, 6}};
    ClassPtrT b[3];

    auto s  = srlz::serialize(a, 3);
    auto rc = srlz::deserialize(b, 3, s.data());

    ASSERT_TRUE(s.to_string() ==
                "{ 0x00, 0x00, 0x00, 0x01, "
                  "0x00, 0x00, 0x00, 0x02, "
                  "0x00, 0x00, 0x00, 0x03, "
                  "0x00, 0x00, 0x00, 0x04, "
                  "0x00, 0x00, 0x00, 0x05, "
                  "0x00, 0x00, 0x00, 0x06 }");
    for (SizeT i = 0; i < 3; ++i) {
        ASSERT_EQ(a[i].a, b[i].a);
        ASSERT_EQ(a[i].b, b[i].b);
    }
    ASSERT_EQ(rc, 24);
}

struct ClassPtrTD {
    U32 size;
    U8* c;

    ClassPtrTD(): size(0), c(nullptr) {}

    ClassPtrTD(const ClassPtrTD& r) {
        size = r.size;

        if (size) {
            c = new U8[r.size];
            memcpy(c, r.c, size);
        } else
            c = nullptr;
    }

    ClassPtrTD(U32 is): size(is) {
        size = is;
        c = new U8[size];

        for (U32 i = 0; i < size; ++i)
            c[i] = U8(i + 1);
    }
    ~ClassPtrTD() {
        delete [] c;
    }

    ClassPtrTD& operator=(const ClassPtrTD& r) {
        if (c == r.c)
            return *this;

        delete [] c;

        size = r.size;

        if (size) {
            c = new U8[r.size];
            memcpy(c, r.c, size);
        } else
            c = nullptr;

        return *this;
    }

    SERIALIZE_METHOD_SIZE() { return SERIALIZE_GET_SIZE_ARRAY(c, size) + SERIALIZE_GET_SIZE(size); }
    SERIALIZE_METHOD     () { SERIALIZE(size); SERIALIZE_ARRAY(c, size); }
    DESERIALIZE_METHOD() {
        DESERIALIZE(size);
        delete [] c;
        c = new U8[size];
        DESERIALIZE_ARRAY(c, size);
    }
};

TEST(Serialization, ClassPtrDynamic) {
    ClassPtrTD a[3] {3, 4, 5};
    ClassPtrTD b[3];
    auto s  = srlz::serialize(a, 3);
    auto rc = srlz::deserialize(b, 3, s.data());


    ASSERT_TRUE(s.to_string() ==
                "{ 0x00, 0x00, 0x00, 0x03, 0x01, 0x02, 0x03, "
                  "0x00, 0x00, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, "
                  "0x00, 0x00, 0x00, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05 }");

    ASSERT_EQ(b[0].size, 3);
    ASSERT_EQ(b[0].c[0], 1);
    ASSERT_EQ(b[0].c[1], 2);
    ASSERT_EQ(b[0].c[2], 3);

    ASSERT_EQ(b[1].size, 4);
    ASSERT_EQ(b[1].c[0], 1);
    ASSERT_EQ(b[1].c[1], 2);
    ASSERT_EQ(b[1].c[2], 3);
    ASSERT_EQ(b[1].c[3], 4);

    ASSERT_EQ(b[2].size, 5);
    ASSERT_EQ(b[2].c[0], 1);
    ASSERT_EQ(b[2].c[1], 2);
    ASSERT_EQ(b[2].c[2], 3);
    ASSERT_EQ(b[2].c[3], 4);
    ASSERT_EQ(b[2].c[4], 5);

    ASSERT_EQ(rc, 24);
}

template <SizeT _Size>
struct ComplexS {
    U64 a;
    ClassPtrTD b;
    ftl::Array<U8, _Size> array;
    ftl::Vector<ClassPtrTD> vec;

    SERIALIZE_METHOD_SIZE() {
        return
        SERIALIZE_GET_SIZE(a) +
        SERIALIZE_GET_SIZE(b) +
        SERIALIZE_GET_SIZE_ARRAY(array.data(), array.size()) +
        SERIALIZE_GET_SIZE(array.size()) +
        SERIALIZE_GET_SIZE_ARRAY(vec.data(), vec.size());
    }
    SERIALIZE_METHOD() {
        SERIALIZE(a);
        SERIALIZE(b);
        SERIALIZE_ARRAY(array.data(), array.size());
        SERIALIZE(vec.size());
        SERIALIZE_ARRAY(vec.data(), vec.size());
    }
    DESERIALIZE_METHOD() {
        DESERIALIZE(a);
        DESERIALIZE(b);
        DESERIALIZE_ARRAY(array.data(), array.size());
        SizeT size;
        DESERIALIZE(size);
        vec.resize(size);
        DESERIALIZE_ARRAY(vec.data(), size);
    }
};

TEST(Serialization, ComplexClassSerialization) {
    auto a = ComplexS<5>();
    a.a = 0x1100220033004400ULL;
    a.b = ClassPtrTD(6);
    a.array = {255, 254, 7, 6, 5};
    a.vec.push_back(ClassPtrTD(8));
    a.vec.push_back(ClassPtrTD(7));
    a.vec.push_back(ClassPtrTD(4));

    auto b = ComplexS<5>();

    auto s  = a.serialize();
    auto rc = b.deserialize(s.data());

    ASSERT_TRUE(s.to_string() ==
        "{ 0x11, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44, 0x00, "
          "0x00, 0x00, 0x00, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, "
          "0xff, 0xfe, 0x07, 0x06, 0x05, "
          "0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, "
          "0x00, 0x00, 0x00, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, "
          "0x00, 0x00, 0x00, 0x07, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, "
          "0x00, 0x00, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04 }");

    ASSERT_TRUE(b.serialize().to_string() == s.to_string());
    ASSERT_EQ(rc, 62);
}