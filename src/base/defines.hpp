#pragma once

#ifndef NDEBUG
    #define DE_DEBUG
#else
    #define DE_RELEASE
#endif

#define repeat(x)  for (auto _REPEAT_MACRO_I = 0; _REPEAT_MACRO_I < (x); ++_REPEAT_MACRO_I)

#define DE_MARK_AS_SINGLETON(CLASSNAME) \
public: \
    CLASSNAME(const CLASSNAME&) = delete; \
    CLASSNAME& operator=(const CLASSNAME&) = delete; \
    static CLASSNAME& instance() { static CLASSNAME inst; return inst; } \
protected: \
    CLASSNAME(); \
    ~CLASSNAME()

#define DE_DEFINE_GETSET(field_name, method_name) \
auto& method_name() const { return field_name; }  \
void  method_name(const decltype(field_name)& value) { field_name = value; }

#define DE_DEFINE_GETSET_FIELD(field_name, method_name) \
auto& method_name() const { return field_name; }  \
auto& method_name() { return field_name; }

#define DE_DEFINE_GET(field_name, getter_name) \
auto& getter_name() const { return field_name; }
