#ifndef DECAYENGINE_LUACONTEXT_HPP
#define DECAYENGINE_LUACONTEXT_HPP

#include <string>

struct lua_State;

namespace lua {

class Context {
    using StrV = std::string_view;
    using Str  = std::string;

public:
    Context(const StrV& path);
    ~Context();

    void doFile        (const StrV& name);
    void addPackagePath(const StrV& path);


    auto luaState() { return L; }

public:
    std::string _packagePath;
    lua_State* L;
};

} // namespace lua

#endif //DECAYENGINE_LUACONTEXT_HPP
