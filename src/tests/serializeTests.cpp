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

TEST(Serialization, U8Ptr) {
    U8 a[6] = {1, 2, 3, 4, 5, 6};

    ASSERT_TRUE(srlz::serialize(a, 6).to_string() == "{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 }");
}

TEST(Serialization, U32Ptr) {
    U32 a[3] = {1, 0xFF00FF00, 0x11002233};

    ASSERT_TRUE(srlz::serialize(a, 3).to_string() ==
                "{ 0x00, 0x00, 0x00, 0x01, 0xff, 0x00, 0xff, 0x00, 0x11, 0x00, 0x22, 0x33 }");
}

struct ClassPtrT {
    U32 a;
    U32 b;
    ClassPtrT(U32 ia, U32 ib): a(ia), b(ib) {}
    SERIALIZE_METHOD_CONST_SIZE() { return sizeof(ClassPtrT); }
    SERIALIZE_METHOD() { SERIALIZE(a); SERIALIZE(b); }
};
TEST(Serialization, ClassPtrStatic) {
    ClassPtrT a[3] {{1, 2}, {3, 4}, {5, 6}};

    ASSERT_TRUE(srlz::serialize(a, 3).to_string() ==
                "{ 0x00, 0x00, 0x00, 0x01, "
                  "0x00, 0x00, 0x00, 0x02, "
                  "0x00, 0x00, 0x00, 0x03, "
                  "0x00, 0x00, 0x00, 0x04, "
                  "0x00, 0x00, 0x00, 0x05, "
                  "0x00, 0x00, 0x00, 0x06 }");
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

    SERIALIZE_METHOD_SIZE() { return sizeof(size) + size; }
    SERIALIZE_METHOD() { SERIALIZE(size); SERIALIZE_ARRAY(c, size); }
};

TEST(Serialization, ClassPtrDynamic) {
    ClassPtrTD a[3] {3, 4, 5};

    ASSERT_TRUE(srlz::serialize(a, 3).to_string() ==
                "{ 0x00, 0x00, 0x00, 0x03, 0x01, 0x02, 0x03, "
                "0x00, 0x00, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, "
                "0x00, 0x00, 0x00, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05 }");
}

template <SizeT _Size>
struct ComplexS {
    U64 a;
    ClassPtrTD b;
    ftl::Array<U8, _Size> array;
    ftl::Vector<ClassPtrTD> vec;

    SERIALIZE_METHOD_SIZE() {
        return
        sizeof(a) +
        b.serialize_size() +
        array.size() +
        sizeof(U32) +
        vec.reduce([](SizeT r, const ClassPtrTD& c) { return r + c.serialize_size(); });
    }
    SERIALIZE_METHOD() {
        SERIALIZE(a);
        SERIALIZE(b);
        SERIALIZE_ARRAY(array.data(), array.size());
        SERIALIZE(U32(vec.size()));
        SERIALIZE_ARRAY(vec.data(), vec.size());
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

    auto str = srlz::serialize(a).to_string();
    ASSERT_TRUE(str ==
        "{ 0x11, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44, 0x00, "
          "0x00, 0x00, 0x00, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, "
          "0xff, 0xfe, 0x07, 0x06, 0x05, "
          "0x00, 0x00, 0x00, 0x03, "
          "0x00, 0x00, 0x00, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, "
          "0x00, 0x00, 0x00, 0x07, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, "
          "0x00, 0x00, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04 }");

    //std::cout << srlz::serialize(a) << std::endl;
}