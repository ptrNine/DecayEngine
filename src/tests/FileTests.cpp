#include <gtest/gtest.h>
#include "../base/files.hpp"


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

#include <filesystem>

TEST(FileTests, Serialize_Deserialize) {
    auto a = ComplexS<5>();
    a.a = 0x1100220033004400ULL;
    a.b = ClassPtrTD(6);
    a.array = {255, 254, 7, 6, 5};
    a.vec.push_back(ClassPtrTD(8));
    a.vec.push_back(ClassPtrTD(7));
    a.vec.push_back(ClassPtrTD(4));

    auto b = ComplexS<5>();

    ftl::String path = std::filesystem::current_path().string() + "/test.txt";

    base::writeBytesToFile(path.data(), a.serialize());
    b.deserialize(base::readFileToBytes(path.data()).data());

    ASSERT_TRUE(a.serialize().to_string() == b.serialize().to_string());
}
