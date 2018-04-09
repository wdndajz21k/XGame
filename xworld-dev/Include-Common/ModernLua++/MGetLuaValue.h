#pragma once
#include "luatable.h"
#include "luafunc.h"

#define DECLARE_M_GET_VALUE(_TYPE, _FUNC, _DEFAULT) \
    template<> inline _TYPE M_GetLuaValue<_TYPE>(lua_State *L, int n) { \
        return n <= lua_gettop(L) ? _FUNC(L, n) : _DEFAULT; \
    }

#define DECLARE_M_GET_REF_VALUE(_TYPE) \
    template<> inline _TYPE M_GetLuaValue<_TYPE>(lua_State *L, int n) \
    { \
        _TYPE LuaRef; \
        if (n <= lua_gettop(L)) \
            LuaRef.setRef(L, n); \
        return LuaRef; \
    }

namespace mLua
{
    template <typename ValueT>
    ValueT M_GetLuaValue(lua_State *L, int n)
    {
        typedef typename M_GET_PURE_TYPE<ValueT>::type PURE_TYPE;
        if (n <= lua_gettop(L))
        {
            LuaUserDataType *ud = (LuaUserDataType*)lua_touserdata(L, n);
            return M_Translate<PURE_TYPE, ValueT>::GetValue(ud->ObjectInstance);
        }
        else
        {
            return ValueT();
        }
    }

    template<>
    inline std::string M_GetLuaValue<std::string>(lua_State *L, int n)
    {
        return n <= lua_gettop(L) ? std::string(lua_tostring(L, n), lua_rawlen(L, n)) : std::string();
    }

    DECLARE_M_GET_VALUE(int, (int)lua_tointeger, 0);
    DECLARE_M_GET_VALUE(int64_t, (int64_t)lua_tointeger, 0);
    DECLARE_M_GET_VALUE(double, lua_tonumber, 0);
    DECLARE_M_GET_VALUE(float, (float)lua_tonumber, 0);
    DECLARE_M_GET_VALUE(bool, !!lua_toboolean, false);
    DECLARE_M_GET_VALUE(const char*, lua_tostring, "");
    DECLARE_M_GET_VALUE(void*, lua_touserdata, nullptr);

    DECLARE_M_GET_REF_VALUE(luaref);
    DECLARE_M_GET_REF_VALUE(luatable);
    DECLARE_M_GET_REF_VALUE(luafunc);

    template<> inline lua_State* M_GetLuaValue<lua_State*>(lua_State *L, int n)
    {
        return L;
    }
}
