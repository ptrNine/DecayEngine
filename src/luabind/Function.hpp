#ifndef DECAYENGINE_LUAFUNCTION_HPP
#define DECAYENGINE_LUAFUNCTION_HPP

#include <string_view>
#include "../base/traits.hpp"
#include "LuaContext.hpp"

extern "C" {
    #include <luajit-2.1/lauxlib.h>
}


#define ICA inline constexpr auto

namespace lua {

namespace details {
    template <typename T>
    struct _returns_count {
        static constexpr std::size_t value = 1;
    };

    template <>
    struct _returns_count<void> {
        static constexpr std::size_t value = 0;
    };

    template <typename... Ts>
    struct _returns_count<std::tuple<Ts...>> {
        static constexpr std::size_t value = sizeof...(Ts);
    };
}

template <typename FunctionT>
class Function {
public:
    using StrV   = std::string_view;
    using Return = ttr::return_type_of<FunctionT>;

    template <std::size_t _Num>
    using Args = ttr::arg_type_of<FunctionT, _Num>;

    static constexpr std::size_t
    _ArgsCount = ttr::args_count<FunctionT>;

    static constexpr std::size_t
    _ReturnsCount = details::_returns_count<Return>::value;


    Function(Context& ctx, const StrV& name) {
        L = ctx.luaState();
        _name = name;
    }

    template <typename... Args>
    auto operator()(Args&&... args) {
        call(args...);

        if constexpr (ttr::numbers<Return>) {
            return getNumber<Return>(-1);
        }
        else if constexpr (std::is_same_v<Return, std::string>) {
            return getString(-1);
        }
        else if constexpr (ttr::is_tuple_v<Return>) {
            Return ret;
            getTupleIter<Return, 0, _ReturnsCount>(ret);
            return std::move(ret);
        }
    }


protected:

    template <typename TupleT, std::size_t cur, std::size_t last, std::enable_if_t<cur == last>...>
    ICA getTupleIter(TupleT& tpl) {
        return tpl;
    }

    template <typename TupleT, std::size_t cur, std::size_t last, std::enable_if_t<cur != last>...>
    ICA getTupleIter(TupleT& tpl) {
        auto& tplElm = std::get<last - cur - 1>(tpl);
        tplElm = getBasicReturn< std::decay_t<decltype(tplElm)> >(-1);

        return getTupleIter<TupleT, cur + 1, last>(tpl);
    }

    template <typename T, std::enable_if_t<ttr::numbers<T>>...>
    ICA getBasicReturn(int num) {
        return getNumber<T>(num);
    }

    template <typename T, std::enable_if_t<std::is_same_v<T, std::string>>...>
    ICA getBasicReturn(int num) {
        return getString(num);
    }

    inline auto getString(int num) {
        if (!lua_isstring(L, num))
            luaL_error(L, "LUA: Function '%s' must return a string", _name.data());

        std::string ret = lua_tostring(L, num);
        lua_pop(L, 1);
        return std::move(ret);
    }

    template <typename T>
    inline auto getNumber(int num) {
        if (!lua_isnumber(L, num))
            luaL_error(L, "LUA: Function '%s' must return a number", _name.data());

        T ret = 0;
        ret = lua_tonumber(L, num);
        lua_pop(L, 1);
        return ret;
    }

    ICA pushArg(double arg)             { lua_pushnumber(L, arg); }
    ICA pushArg(int arg)                { lua_pushinteger(L, arg); }
    ICA pushArg(bool arg)               { lua_pushboolean(L, arg); }
    ICA pushArg(const std::string& arg) { lua_pushstring(L, arg.data()); }
    ICA pushArg(const char* arg)        { lua_pushstring(L, arg); }


    template <typename... Args>
    ICA call(Args&&... args) {
        lua_getglobal(L, _name.data());

        static_assert(sizeof...(Args) == _ArgsCount, "Wrong number of arguments");
        (pushArg(args), ...);

        if (lua_pcall(L, static_cast<int>(_ArgsCount), static_cast<int>(_ReturnsCount), 0))
            luaL_error(L, "LUA: Error running '%s' function. %s", _name.data(), lua_tostring(L, -1));
    }

protected:
    std::string _name;
    lua_State* L;
};


template <typename FunctionT>
class Method {


protected:
};

} // namespace lua

#undef ICA // inline constexpr auto

#endif //DECAYENGINE_LUAFUNCTION_HPP
