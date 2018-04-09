#pragma once

#include "MGlobal.h"
#include "luaref.h"
#include "luaret.h"
#include "luafunc.h"
#include "luatable.h"
#include "LuaTableField.h"
#include "MRegisterToLua.h"

#define M_LUA

namespace mLua
{
    M_API lua_State* M_New();
    M_API void M_Destroy(lua_State *L);
    M_API void M_Init(lua_State *L);
    M_API void M_UnInit(lua_State *L);
}
