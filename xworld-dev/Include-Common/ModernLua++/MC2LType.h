#pragma once
#include "luatable.h"
#include "luafunc.h"
#include "MPureTypeHelper.h"

namespace mLua
{
    template <typename T>
    char CType2LuaType()
    {
        // compile error, please check the type of function param
        typedef typename M_GET_PURE_TYPE<T>::type PURE_TYPE;
        PURE_TYPE::M_S_GetClassName();
        return LUA_SLE_TOBJECT;
    }

    template <> char inline CType2LuaType<int>()            { return LUA_TNUMBER; }
    template <> char inline CType2LuaType<unsigned int>()   { return LUA_TNUMBER; }
    template <> char inline CType2LuaType<double>()         { return LUA_TNUMBER; }
    template <> char inline CType2LuaType<float>()          { return LUA_TNUMBER; }
    template <> char inline CType2LuaType<bool>()           { return LUA_TBOOLEAN; }
    template <> char inline CType2LuaType<const char*>()    { return LUA_TSTRING; }
    template <> char inline CType2LuaType<char*>()          { return LUA_TSTRING; }
    template <> char inline CType2LuaType<std::string>()    { return LUA_TSTRING; }
    template <> char inline CType2LuaType<void*>()          { return LUA_TUSERDATA; }
    template <> char inline CType2LuaType<luatable>()       { return LUA_TTABLE; }
    template <> char inline CType2LuaType<luafunc>()        { return LUA_TFUNCTION; }
    template <> char inline CType2LuaType<lua_State*>()     { return LUA_SLE_TSTATE; }
}
