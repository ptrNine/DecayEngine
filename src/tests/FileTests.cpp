#include <gtest/gtest.h>

#include "../base/files.hpp"

struct S {
    int a;
    int b;
    float c;

    SERIALIZE_METHOD_CONST_SIZE() {
        return sizeof(S);
    }

    SERIALIZE_METHOD() {
        SERIALIZE(a);
        SERIALIZE(b);
        SERIALIZE(c);
    }
};

struct A {
    S s;
    int b;

    SERIALIZE_METHOD_CONST_SIZE() {
        return sizeof(A);
    }

    SERIALIZE_METHOD() {
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