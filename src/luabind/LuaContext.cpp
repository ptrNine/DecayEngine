#include "LuaContext.hpp"

extern "C" {
    #include <luajit-2.1/lua.h>
    #include <luajit-2.1/lauxlib.h>
    #include <luajit-2.1/lualib.h>
    #include <luajit-2.1/luajit.h>
}

namespace lua {

Context::Context(const Context::StrV &path)
{
    L = luaL_newstate();
    luaL_openlibs(L);
    addPackagePath(path);
}

Context::~Context()
{
    lua_close(L);
}

void Context::doFile(const Context::StrV &name)
{
    luaL_dofile(L, (_packagePath + "/" + name.data()).data());
}

void Context::addPackagePath(const Context::StrV &path)
{
    _packagePath = path;

    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");

    std::string s = luaL_checkstring(L, -1);
    s += Str(";") + path.data() + Str("/?.lua");

    lua_pushstring(L, s.c_str());
    lua_setfield(L, -3, "path");
    lua_pop(L, 2);
}

} // namespace lua