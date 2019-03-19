#include "../base/ftl/string.hpp"
#include "../base/ftl/vector.hpp"

#include "gtest/gtest.h"

TEST(StringTests, CompileTimeString) {
    constexpr auto str      = CS("string");
    constexpr auto size     = str.size();
    constexpr auto c_str    = str.c_str();
    constexpr auto str_view = str.str_view();
    constexpr auto hash     = str.hash();
    constexpr auto cut_l    = str.cut_l<2>();
    constexpr auto cut_r    = str.cut_r<3>();
    constexpr auto first    = str[0];
    constexpr auto last     = str.get<5>();
    constexpr auto new_str  = CS("it's a ") + CS("compile-time ") + str;
    constexpr auto testF    = CS("").front();
    constexpr auto testB    = CS("").back();

    constexpr auto path1    = str / CS("path");
    constexpr auto path2    = str / CS("/path");
    constexpr auto path3    = (str + CS("/")) / CS("/path");


    ASSERT_EQ(size, 6);
    ASSERT_STREQ(c_str, "string");
    ASSERT_EQ(std::string_view("string"), str_view);
    ASSERT_EQ(hash, 0x704be0d8faaffc58ULL);
    ASSERT_EQ(cut_l, CS("ring"));
    ASSERT_EQ(cut_r, CS("str"));
    ASSERT_EQ(first, 's');
    ASSERT_EQ(last, 'g');
    ASSERT_EQ(new_str, CS("it's a compile-time string"));
    ASSERT_EQ(path1, CS("string/path"));
    ASSERT_EQ(path2, CS("string/path"));
    ASSERT_EQ(path3, CS("string/path"));
}

TEST(StringTests, CompileTimeString16) {
    constexpr auto str      = CS(u"string");
    constexpr auto size     = str.size();
    constexpr auto c_str    = str.c_str();
    constexpr auto str_view = str.str_view();
    constexpr auto hash     = str.hash();
    constexpr auto cut_l    = str.cut_l<2>();
    constexpr auto cut_r    = str.cut_r<3>();
    constexpr auto first    = str[0];
    constexpr auto last     = str.get<5>();
    constexpr auto new_str  = CS(u"it's a ") + CS(u"compile-time ") + str;
    constexpr auto testF    = CS(u"").front();
    constexpr auto testB    = CS(u"").back();

    constexpr auto path1    = str / CS(u"path");
    constexpr auto path2    = str / CS(u"/path");
    constexpr auto path3    = (str + CS(u"/")) / CS(u"/path");


    ASSERT_EQ(size, 6);
    ASSERT_EQ(std::basic_string_view<Char16>(u"string"), str_view);
    ASSERT_EQ(hash, 0xA2B41F275C61261EULL);
    ASSERT_EQ(cut_l, CS(u"ring"));
    ASSERT_EQ(cut_r, CS(u"str"));
    ASSERT_EQ(first, u's');
    ASSERT_EQ(last, u'g');
    ASSERT_EQ(new_str, CS(u"it's a compile-time string"));
    ASSERT_EQ(path1, CS(u"string/path"));
    ASSERT_EQ(path2, CS(u"string/path"));
    ASSERT_EQ(path3, CS(u"string/path"));
}

TEST(StringTests, CompileTimeString32) {
    constexpr auto str      = CS(U"string");
    constexpr auto size     = str.size();
    constexpr auto c_str    = str.c_str();
    constexpr auto str_view = str.str_view();
    constexpr auto hash     = str.hash();
    constexpr auto cut_l    = str.cut_l<2>();
    constexpr auto cut_r    = str.cut_r<3>();
    constexpr auto first    = str[0];
    constexpr auto last     = str.get<5>();
    constexpr auto new_str  = CS(U"it's a ") + CS(U"compile-time ") + str;
    constexpr auto testF    = CS(U"").front();
    constexpr auto testB    = CS(U"").back();

    constexpr auto path1    = str / CS(U"path");
    constexpr auto path2    = str / CS(U"/path");
    constexpr auto path3    = (str + CS(U"/")) / CS(U"/path");


    ASSERT_EQ(size, 6);
    ASSERT_EQ(std::basic_string_view<Char32>(U"string"), str_view);
    ASSERT_EQ(hash, 0x7493BFED564CDDE6ULL);
    ASSERT_EQ(cut_l, CS(U"ring"));
    ASSERT_EQ(cut_r, CS(U"str"));
    ASSERT_EQ(first, U's');
    ASSERT_EQ(last, U'g');
    ASSERT_EQ(new_str, CS(U"it's a compile-time string"));
    ASSERT_EQ(path1, CS(U"string/path"));
    ASSERT_EQ(path2, CS(U"string/path"));
    ASSERT_EQ(path3, CS(U"string/path"));
}

TEST(StringTests, StringBase) {
    using ftl::String;

    String str1("string");
    String str2(CS("string"));
    String str3(std::string_view("string"));

    auto cstr1 = str1.c_str();
    auto cstr2 = str2.c_str();
    auto cstr3 = str3.c_str();

    auto size1 = str1.lenght();
    auto size2 = str2.lenght();
    auto size3 = str3.lenght();

    auto newStr0 = str1 + String(" heh");
    auto newStr1 = str1 + " heh";
    auto newStr2 = str1 + CS(" heh");

    auto vec = ftl::Vector<char>{'v', 's', 't', 'r'};
    auto newStr3 = String(vec.cbegin(), vec.cend());
    auto newStr4 = String(vec.crbegin(), vec.crend());

    auto path1 = String("path")  / String("test");
    auto path2 = String("path")  / String("/test");
    auto path3 = String("path/") / String("test");
    auto path4 = String("path/") / String("/test");

    auto spath1 = String("path")  / "test/";
    auto spath2 = String("path")  / "/test/";
    auto spath3 = String("path/") / "test/";
    auto spath4 = String("path/") / "/test/";

    auto cpath1 = String("path")  / CS("test/");
    auto cpath2 = String("path")  / CS("/test/");
    auto cpath3 = String("path/") / CS("test/");
    auto cpath4 = String("path/") / CS("/test/");

    ASSERT_TRUE(str1 == "string");
    ASSERT_EQ(str2, CS("string"));
    ASSERT_EQ(str3, String("string"));

    ASSERT_EQ(size1, 6);
    ASSERT_EQ(size2, 6);
    ASSERT_EQ(size3, 6);

    ASSERT_EQ(newStr0, "string heh");
    ASSERT_EQ(newStr1, CS("string heh"));
    ASSERT_EQ(newStr2, String("string heh"));
    ASSERT_EQ(newStr3, "vstr");
    ASSERT_EQ(newStr4, "rtsv");

    ASSERT_EQ(path1, "path/test");
    ASSERT_EQ(path2, "path/test");
    ASSERT_EQ(path3, "path/test");
    ASSERT_EQ(path4, "path/test");

    ASSERT_EQ(spath1, "path/test/");
    ASSERT_EQ(spath2, "path/test/");
    ASSERT_EQ(spath3, "path/test/");
    ASSERT_EQ(spath4, "path/test/");

    ASSERT_EQ(cpath1, "path/test/");
    ASSERT_EQ(cpath2, "path/test/");
    ASSERT_EQ(cpath3, "path/test/");
    ASSERT_EQ(cpath4, "path/test/");

    path1 /= String("/kek");
    path2 /= String("/kek");
    path3 /= String("kek");
    path4 /= String("kek");

    spath1 /= "/kek";
    spath2 /= "/kek";
    spath3 /= "kek";
    spath4 /= "kek";

    cpath1 /= CS("/kek");
    cpath2 /= CS("/kek");
    cpath3 /= CS("kek");
    cpath4 /= CS("kek");

    ASSERT_EQ(path1, "path/test/kek");
    ASSERT_EQ(path2, "path/test/kek");
    ASSERT_EQ(path3, "path/test/kek");
    ASSERT_EQ(path4, "path/test/kek");

    ASSERT_EQ(spath1, "path/test/kek");
    ASSERT_EQ(spath2, "path/test/kek");
    ASSERT_EQ(spath3, "path/test/kek");
    ASSERT_EQ(spath4, "path/test/kek");

    ASSERT_EQ(cpath1, "path/test/kek");
    ASSERT_EQ(cpath2, "path/test/kek");
    ASSERT_EQ(cpath3, "path/test/kek");
    ASSERT_EQ(cpath4, "path/test/kek");

    ASSERT_EQ(String().assign_c_str("str"), "str");
    ASSERT_EQ(String().assign_c_str("str", 3), "str");
}


TEST(StringTests, StringBase16) {
    using String = ftl::String16;

    String str1(u"string");
    String str2(CS(u"string"));
    String str3(std::basic_string_view(u"string"));

    auto cstr1 = str1.c_str();
    auto cstr2 = str2.c_str();
    auto cstr3 = str3.c_str();

    auto size1 = str1.lenght();
    auto size2 = str2.lenght();
    auto size3 = str3.lenght();

    auto newStr0 = str1 + String(u" heh");
    auto newStr1 = str1 + u" heh";
    auto newStr2 = str1 + CS(u" heh");

    auto vec = ftl::Vector<Char16>{u'v', u's', u't', u'r'};
    auto newStr3 = String(vec.cbegin(), vec.cend());
    auto newStr4 = String(vec.crbegin(), vec.crend());

    auto path1 = String(u"path")  / String(u"test");
    auto path2 = String(u"path")  / String(u"/test");
    auto path3 = String(u"path/") / String(u"test");
    auto path4 = String(u"path/") / String(u"/test");

    auto spath1 = String(u"path")  / u"test/";
    auto spath2 = String(u"path")  / u"/test/";
    auto spath3 = String(u"path/") / u"test/";
    auto spath4 = String(u"path/") / u"/test/";

    auto cpath1 = String(u"path")  / CS(u"test/");
    auto cpath2 = String(u"path")  / CS(u"/test/");
    auto cpath3 = String(u"path/") / CS(u"test/");
    auto cpath4 = String(u"path/") / CS(u"/test/");

    ASSERT_EQ(str1, u"string");
    ASSERT_EQ(str2, CS(u"string"));
    ASSERT_EQ(str3, String(u"string"));

    ASSERT_EQ(size1, 6);
    ASSERT_EQ(size2, 6);
    ASSERT_EQ(size3, 6);

    ASSERT_EQ(newStr0, u"string heh");
    ASSERT_EQ(newStr1, CS(u"string heh"));
    ASSERT_EQ(newStr2, String(u"string heh"));
    ASSERT_EQ(newStr3, u"vstr");
    ASSERT_EQ(newStr4, u"rtsv");

    ASSERT_EQ(path1,  u"path/test");
    ASSERT_EQ(path2,  u"path/test");
    ASSERT_EQ(path3,  u"path/test");
    ASSERT_EQ(path4,  u"path/test");
    ASSERT_EQ(spath1, u"path/test/");
    ASSERT_EQ(spath2, u"path/test/");
    ASSERT_EQ(spath3, u"path/test/");
    ASSERT_EQ(spath4, u"path/test/");
    ASSERT_EQ(cpath1, u"path/test/");
    ASSERT_EQ(cpath2, u"path/test/");
    ASSERT_EQ(cpath3, u"path/test/");
    ASSERT_EQ(cpath4, u"path/test/");

    path1 /= String(u"/kek");
    path2 /= String(u"/kek");
    path3 /= String(u"kek");
    path4 /= String(u"kek");

    spath1 /= u"/kek";
    spath2 /= u"/kek";
    spath3 /= u"kek";
    spath4 /= u"kek";

    cpath1 /= CS(u"/kek");
    cpath2 /= CS(u"/kek");
    cpath3 /= CS(u"kek");
    cpath4 /= CS(u"kek");

    ASSERT_EQ(path1, u"path/test/kek");
    ASSERT_EQ(path2, u"path/test/kek");
    ASSERT_EQ(path3, u"path/test/kek");
    ASSERT_EQ(path4, u"path/test/kek");

    ASSERT_EQ(spath1, u"path/test/kek");
    ASSERT_EQ(spath2, u"path/test/kek");
    ASSERT_EQ(spath3, u"path/test/kek");
    ASSERT_EQ(spath4, u"path/test/kek");

    ASSERT_EQ(cpath1, u"path/test/kek");
    ASSERT_EQ(cpath2, u"path/test/kek");
    ASSERT_EQ(cpath3, u"path/test/kek");
    ASSERT_EQ(cpath4, u"path/test/kek");
}

TEST(StringTests, Algorithm) {
    using String = ftl::StringBase<char>;
    auto str1  = String("string");
    auto str2  = String("string");
    auto str3  = String("string");
    auto str4  = String("string");
    auto str5  = String("string");
    auto str6  = String("string");
    auto str7  = String("string");
    auto str8  = String("string");
    auto str9  = String("string");
    auto str10 = String("string");
    auto str11 = String("string");
    auto str12 = String("string");
    auto str13 = String("string");
    auto str14 = String("string");
    auto str15 = String("string");
    auto str16 = String("string");
    auto str17 = String("string");
    auto str18 = String("string");
    auto str19 = String("string");
    auto str20 = String("string");
    auto str21 = String("string");
    auto str22 = String("string");
    auto str23 = String("string");

    str1.insert(str1.begin() + 1, 2, 'E');
    ASSERT_EQ(str1, "sEEtring");

    auto vec = ftl::Vector{'1', '2'};
    str2.insert(str2.begin() + 1, vec.begin(), vec.end());
    ASSERT_EQ(str2, "s12tring");

    str3.insert(str3.end(), '1');
    ASSERT_EQ(str3, "string1");

    str4.insert(str4.begin(), {'1', '2', '3'});
    ASSERT_EQ(str4, "123string");

    str5.insert(1, str5);
    ASSERT_EQ(str5, "sstringtring");

    str6.insert(3, str6, 1, 2);
    ASSERT_EQ(str6, "strtring");

    str7.insert(3, "str");
    ASSERT_EQ(str7, "strstring");

    str7.insert(1, "");
    ASSERT_EQ(str7, "strstring");

    str8.insert(3, "kaaakk", 1, 3);
    ASSERT_EQ(str8, "straaaing");

    str9.insert(2, 5, 'A');
    ASSERT_EQ(str9, "stAAAAAring");

    str10.insert(3, CS("str"));
    ASSERT_EQ(str10, "strstring");

    str11.insert(3, CS("kaaakk"), 1, 3);
    ASSERT_EQ(str11, "straaaing");

    str11.erase(1, 1);
    ASSERT_EQ(str11, "sraaaing");

    str11.erase(str11.end() - 1);
    ASSERT_EQ(str11, "sraaain");

    str11.erase(0, 1);
    str11.erase(str11.begin() + 1, str11.begin() + 3);
    ASSERT_EQ(str11, "rain");

    str12.replace(1, 2, String("ab"));
    ASSERT_EQ(str12, "sabing");

    str13.replace(1, 2, String("aakeaa"), 2, 2);
    ASSERT_EQ(str13, "skeing");

    str14.replace(1, 2, "aakeaa", 2, 2);
    ASSERT_EQ(str14, "skeing");

    str15.replace(1, 2, CS("aakeaa"), 2, 3);
    ASSERT_EQ(str15, "skeaing");

    str16.replace(1, 2, 5, 'k');
    ASSERT_EQ(str16, "skkkkking");

    str17.replace(str17.begin(), str17.begin() + 2, String("kekkek"));
    ASSERT_EQ(str17, "kekkekring");

    str18.replace(str18.begin(), str18.begin() + 2, "kekkek");
    ASSERT_EQ(str18, "kekkekring");

    str19.replace(str19.begin(), str19.begin() + 2, CS("kekkek"));
    ASSERT_EQ(str19, "kekkekring");

    str18.replace(str18.begin(), str18.begin() + 2, "abcd", 1, 2);
    ASSERT_EQ(str18, "bckkekring");

    str19.replace(str19.begin(), str19.begin() + 2, CS("abcd"), 1, 2);
    ASSERT_EQ(str19, "bckkekring");

    str20.replace(str20.begin() + 1, str20.begin() + 3, 5, 'k');
    ASSERT_EQ(str20, "skkkkking");

    str20.replace(str20.begin() + 1, str20.end(), str21.begin() + 1, str21.end());
    ASSERT_EQ(str21, "string");

    str21.replace(str21.begin(), str21.begin() + 2, {'1', '2', '3'});
    ASSERT_EQ(str21, "123ring");

    ASSERT_EQ(3, str22.find("in"));
    ASSERT_EQ(3, str22.find(CS("in")));
    ASSERT_EQ(3, str22.find(String("in")));
    ASSERT_EQ(3, str22.find('i'));

    ASSERT_EQ(3, str22.rfind("in"));
    ASSERT_EQ(3, str22.rfind(CS("in")));
    ASSERT_EQ(3, str22.rfind(String("in")));
    ASSERT_EQ(3, str22.rfind('i'));

    ASSERT_EQ(3, str22.find_first_of("in"));
    ASSERT_EQ(3, str22.find_first_of(CS("in")));
    ASSERT_EQ(3, str22.find_first_of(String("in")));
    ASSERT_EQ(3, str22.find_first_of('i'));

    ASSERT_EQ(4, str22.find_last_of("ni"));
    ASSERT_EQ(4, str22.find_last_of(CS("ni")));
    ASSERT_EQ(4, str22.find_last_of(String("ni")));
    ASSERT_EQ(3, str22.find_last_of('i'));

    ASSERT_EQ(3, str22.find_first_not_of("strng"));
    ASSERT_EQ(3, str22.find_first_not_of(CS("strng")));
    ASSERT_EQ(3, str22.find_first_not_of(String("strng")));
    ASSERT_EQ(1, str22.find_first_not_of('s'));

    ASSERT_EQ(3, str22.find_last_not_of("strng"));
    ASSERT_EQ(3, str22.find_last_not_of(CS("strng")));
    ASSERT_EQ(3, str22.find_last_not_of(String("strng")));
    ASSERT_EQ(4, str22.find_last_not_of('g'));
}