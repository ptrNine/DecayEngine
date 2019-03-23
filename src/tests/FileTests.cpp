#include <gtest/gtest.h>

#include "../base/files.hpp"

struct S {
    int a;
    int b;
    float c;

    constexpr auto c_serialize_size() const {
        return sizeof(S);
    }

    void serialize_impl(Byte* ptr) const {
        SERIALIZE_START(ptr);
        SERIALIZE(a);
        SERIALIZE(b);
        SERIALIZE(c);
    }
};

struct A {
    S s;
    int b;

    constexpr auto c_serialize_size() const {
        return sizeof(A);
    }

    void serialize_impl(Byte* ptr) const {
        SERIALIZE_START(ptr);
        SERIALIZE(s);
        SERIALIZE(b);
    }
};

TEST(Files, FileWriter) {
    S s;
    s.a = 256;
    s.b = 0x11001100;
    s.c = 24243.f;

    A a;
    a.s = s;
    a.b = 2313;

    base::FileWriter file ("/mnt/sda6-drive/Repos/DecayEngine/aaa/bbbb/text.txt");
    file.write(a);
}