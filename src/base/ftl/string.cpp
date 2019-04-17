#include "string.hpp"
#include "vector.hpp"

#define SPLIT_VIEW_IMPL(CHAR_T)                                        \
template <>                                                            \
auto StringBase<CHAR_T>::splitView(CHAR_T c) const -> Vector<StrView> {\
    Vector<StrView> vec;                                               \
    StrView data = _str_v;                                             \
    SizeType start = 0;                                                \
                                                                       \
    for (SizeType i = 0; i < data.size(); ++i) {                       \
        if (data[i] == c) {                                            \
            if (start != i) {                                          \
                vec.emplace_back(data.substr(start, i - start));       \
            }                                                          \
            start = i + 1;                                             \
        }                                                              \
    }                                                                  \
    if (start != data.size())                                          \
        vec.emplace_back(data.substr(start, data.size()));             \
    return std::move(vec);                                             \
}

#define SPLIT_VIEW_STR_IMPL(CHAR_T)                                               \
template<>                                                                        \
auto StringBase<CHAR_T>::splitView(const StrView& str) const -> Vector<StrView> { \
    Vector<SizeType> idxs;                                                        \
    idxs.reserve(128);                                                            \
                                                                                  \
    Vector<StrView> strs;                                                         \
    StrView data = _str_v;                                                        \
                                                                                  \
    { PtrDiff pos = -str.length();                                                \
        while ((pos = data.find(str, pos + str.length())) != StrView::npos)       \
            idxs.push_back(static_cast<SizeType>(pos)); }                         \
                                                                                  \
    strs.reserve(idxs.size() + 1);                                                \
                                                                                  \
    SizeType pos = 0;                                                             \
    for (auto idx : idxs) {                                                       \
        if (pos != idx)                                                           \
            strs.emplace_back(data.substr(pos, idx - pos));                       \
        pos = idx + str.length();                                                 \
    }                                                                             \
    if (pos != data.length())                                                     \
        strs.emplace_back(data.substr(pos, data.length()));                       \
    return std::move(strs);                                                       \
}

#define SPLIT_VIEW_IL_IMPL(CHAR_T)                                     \
template <>                                                            \
auto StringBase<CHAR_T>::splitView                                     \
(std::initializer_list<CHAR_T> l, bool createNullStrs)                 \
const -> Vector<StrView> {                                             \
    Vector<StrView> vec;                                               \
    StrView data = _str_v;                                             \
    SizeType start = 0;                                                \
                                                                       \
    for (SizeType i = 0; i < data.size(); ++i) {                       \
        bool cmp = false;                                              \
        for (auto c : l) { if (data[i] == c) { cmp = true; break; } }  \
        if (cmp) {                                                     \
            if (createNullStrs || start != i) {                        \
                vec.emplace_back(data.substr(start, i - start));       \
            }                                                          \
            start = i + 1;                                             \
        }                                                              \
    }                                                                  \
    if (start != data.size())                                          \
        vec.emplace_back(data.substr(start, data.size()));             \
    return std::move(vec);                                             \
}

#define SPLIT_IMPL(CHAR_T)                                                         \
template <>                                                                        \
auto StringBase<CHAR_T>::split(CHAR_T c) const -> Vector<StringBase<CHAR_T>> {     \
    Vector<StringBase<CHAR_T>> vec;                                                \
    StrView data = _str_v;                                                         \
    SizeType start = 0;                                                            \
                                                                                   \
    for (SizeType i = 0; i < data.size(); ++i) {                                   \
        if (data[i] == c) {                                                        \
            if (start != i) {                                                      \
                vec.emplace_back(data.substr(start, i - start));                   \
            }                                                                      \
            start = i + 1;                                                         \
        }                                                                          \
    }                                                                              \
    if (start != data.size())                                                      \
        vec.emplace_back(data.substr(start, data.size()));                         \
    return std::move(vec);                                                         \
}

#define SPLIT_STR_IMPL(CHAR_T)                                                           \
template<>                                                                                    \
auto StringBase<CHAR_T>::split(const StrView& str) const -> Vector<StringBase<CHAR_T>> {      \
    Vector<SizeType> idxs;                                                                    \
    idxs.reserve(128);                                                                        \
                                                                                              \
    Vector<StringBase<CHAR_T>> strs;                                                          \
    StrView data = _str_v;                                                                    \
                                                                                              \
    { PtrDiff pos = -str.length();                                                            \
        while ((pos = data.find(str, pos + str.length())) != StrView::npos)                   \
            idxs.push_back(static_cast<SizeType>(pos)); }                                     \
                                                                                              \
    strs.reserve(idxs.size() + 1);                                                            \
                                                                                              \
    SizeType pos = 0;                                                                         \
    for (auto idx : idxs) {                                                                   \
        if (pos != idx)                                                                       \
            strs.emplace_back(data.substr(pos, idx - pos));                                   \
        pos = idx + str.length();                                                             \
    }                                                                                         \
    if (pos != data.length())                                                                 \
        strs.emplace_back(data.substr(pos, data.length()));                                   \
    return std::move(strs);                                                                   \
}

#define SPLIT_IL_IMPL(CHAR_T)                                          \
template <>                                                            \
auto StringBase<CHAR_T>::split                                         \
(std::initializer_list<CHAR_T> l, bool createNullStrs)                 \
const -> Vector<StringBase<CHAR_T>> {                                  \
    Vector<StringBase<CHAR_T>> vec;                                    \
    StrView data = _str_v;                                             \
    SizeType start = 0;                                                \
                                                                       \
    for (SizeType i = 0; i < data.size(); ++i) {                       \
        bool cmp = false;                                              \
        for (auto c : l) { if (data[i] == c) { cmp = true; break; } }  \
        if (cmp) {                                                     \
            if (createNullStrs || start != i) {                        \
                vec.emplace_back(data.substr(start, i - start));       \
            }                                                          \
            start = i + 1;                                             \
        }                                                              \
    }                                                                  \
    if (start != data.size())                                          \
        vec.emplace_back(data.substr(start, data.size()));             \
    return std::move(vec);                                             \
}

namespace ftl {
    SPLIT_IMPL(Char8)
    SPLIT_IMPL(Char16)
    SPLIT_IMPL(Char32)

    SPLIT_STR_IMPL(Char8)
    SPLIT_STR_IMPL(Char16)
    SPLIT_STR_IMPL(Char32)

    SPLIT_IL_IMPL(Char8)
    SPLIT_IL_IMPL(Char16)
    SPLIT_IL_IMPL(Char32)

    SPLIT_VIEW_IMPL(Char8)
    SPLIT_VIEW_IMPL(Char16)
    SPLIT_VIEW_IMPL(Char32)

    SPLIT_VIEW_STR_IMPL(Char8)
    SPLIT_VIEW_STR_IMPL(Char16)
    SPLIT_VIEW_STR_IMPL(Char32)

    SPLIT_VIEW_IL_IMPL(Char8)
    SPLIT_VIEW_IL_IMPL(Char16)
    SPLIT_VIEW_IL_IMPL(Char32)
}