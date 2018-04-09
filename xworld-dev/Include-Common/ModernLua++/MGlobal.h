#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <string.h>
#include <functional>
#include "Lua/lua.hpp"

#pragma warning(disable:4996)

#define M_API

#define LUA_TYPE_COUNT		11		// lua类型数量
#define LUA_SLE_TSTATE      11      // lua_State*
#define LUA_SLE_TOBJECT     12      // 所有从luaclass派生出的类，即脚本对象
#define LUA_SLE_TVARIABLE   13      // 任意lua类型

#define SLE_DELETE(p) do { if (p) delete p; p = NULL; } while(false)
#define SLE_DELETE_ARRAY(p) do { if (p) delete[] p; p = NULL; } while(false)

#define SLE_MAX_PARAMS_COUNT 12

#define SLEClassTable "__SLEClass"
#define SLEStaticClassOverrideTable "__StaticOverride"
#define SLEMemberClassOverrideTable "__MemberOverride"
#define SLEClassNameField "__ClassName"

namespace mLua
{
    typedef std::function<bool(lua_State *L)> M_TableTravCallBack;
    typedef std::function<void(const char *err)> M_OnErrorHandle;
    class LuaAttachBase;
    struct LuaUserDataType
    {
        void *ObjectInstance;
        char ClassName[64];
        LuaAttachBase *Protector;
    };

    M_API class LuaStackHolder
    {
    public:
        LuaStackHolder(lua_State *L);
        ~LuaStackHolder();
    protected:
        lua_State *ThisL;
        int SaveTopIndex;
    };

    M_API bool SLE_GetValue(lua_State *L, const char *addr);
    M_API int SLE_PCall(lua_State *L, int argCount, int resultCount, int *outResultCount);
    M_API int SLE_PCall_OnError(lua_State *L);
    M_API bool SLE_OnError(lua_State *L);
    M_API void SLE_ThrowError(lua_State *L, const char *err);
    template <typename ... ArgsType>
    M_API void SLE_ThrowError(lua_State *L, const char *errFmt, ArgsType ... args)
    {
        char err[1024];
        sprintf(err, errFmt, args...);
        SLE_ThrowError(L, err);
    }

    M_API void M_Traversal(lua_State *L, int n, M_TableTravCallBack callback);
    M_API const char* M_LoadScript(lua_State *L, const char *lua, const char *chunkName);
    M_API const char *LuaTypeName(int luaType);
    M_API void M_SetOnErrorHandle(const M_OnErrorHandle& callback);

    M_API class MNil {};
    M_API MNil nil();
}
