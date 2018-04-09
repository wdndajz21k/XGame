#pragma once
#include "LuaAttach.h"
#include <tuple>

#pragma warning(push)
#pragma warning(disable:4244)

#define M_DECLARE_PUSH(_TYPE, _METHOD) M_API inline void M_Push(lua_State *L, _TYPE value) { _METHOD(L, value); }

namespace mLua
{
    M_API class luaref;
    M_API class luatable;
    M_API class luafunc;

    M_DECLARE_PUSH(int, lua_pushinteger);
    M_DECLARE_PUSH(float, lua_pushnumber);
    M_DECLARE_PUSH(int64_t, lua_pushnumber);
    M_DECLARE_PUSH(double, lua_pushnumber);
    M_DECLARE_PUSH(bool, lua_pushboolean);
    M_DECLARE_PUSH(const char*, lua_pushstring);
    M_DECLARE_PUSH(lua_CFunction, lua_pushcfunction);
    M_DECLARE_PUSH(void*, lua_pushlightuserdata);
    M_API inline void M_Push(lua_State *L, MNil value) { lua_pushnil(L); }

    M_API void M_Push(lua_State *L, luaref& value);
    M_API void M_Push(lua_State *L, luatable& value);
    M_API void M_Push(lua_State *L, luafunc& value);
    M_API void M_Push(lua_State *L, std::string *value);
    M_API void M_Push(lua_State *L, const std::string& value);

    template <int N>
    M_API void inline M_Push(lua_State *L, const char value[N]) { lua_pushstring(L, value); }

    template <typename ClassT>
    inline void M_Push(lua_State *L, ClassT *obj)
    {
        if (!obj)
        {
            lua_pushnil(L);
        }
        else
        {
            LuaUserDataType *ud = (LuaUserDataType*)lua_newuserdata(L, sizeof(LuaUserDataType));
            strcpy(ud->ClassName, obj->M_GetClassName());
            ud->ObjectInstance = obj;
            ud->Protector = &obj->__PointerProtector;
            assert(ud->Protector->CheckCode == LUA_ATTACH_CHECK_CODE);
            ud->Protector->AddRef(ud);
            luaL_getmetatable(L, ud->ClassName);
            assert(lua_type(L, -1) == LUA_TTABLE);
            lua_setmetatable(L, -2);
        }
    }

    template <typename ClassT>
    inline void M_Push(lua_State *L, ClassT& rObj)
    {
        M_Push(L, &rObj);
    }

    M_API void M_Push(lua_State *L, void *obj, const char *className);

    M_API void inline M_BatchPush(lua_State *L) { }

    template <typename FirstType, typename ... Arguments>
    M_API void inline M_BatchPush(lua_State *L, FirstType first, Arguments ... args)
    {
        M_Push(L, first);
        M_BatchPush(L, args...);
    }

    template<class Tuple, std::size_t N>
    struct LuaPushTuple
    {
        static void Push(lua_State *L, Tuple& t)
        {
            LuaPushTuple<Tuple, N - 1>::Push(L, t);
            M_Push(L, std::get<N - 1>(t));
        }
    };

    template<class Tuple>
    struct LuaPushTuple<Tuple, 1>
    {
        static void Push(lua_State *L, Tuple& t)
        {
            M_Push(L, std::get<0>(t));
        }
    };

    template<class... Args>
    void M_Push(lua_State *L, std::tuple<Args...>& t)
    {
        LuaPushTuple<decltype(t), sizeof...(Args)>::Push(L, t);
    }
}

#pragma warning(pop)
