#ifndef DECAYENGINE_LANG_DEFINES_HPP
#define DECAYENGINE_LANG_DEFINES_HPP

#ifndef NDEBUG
    #define DE_DEBUG
#else
    #define DE_RELEASE
#endif

#define repeat(x)  for (auto _REPEAT_MACRO_I = 0; _REPEAT_MACRO_I < (x); ++_REPEAT_MACRO_I)

#endif //DECAYENGINE_LANG_DEFINES_HPP
