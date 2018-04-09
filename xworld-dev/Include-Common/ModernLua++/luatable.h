#pragma once
#include "luaref.h"
#include "LuaTableField.h"
#include <functional>

namespace mLua
{
    M_API class luatable : public mLua::luaref
    {
        friend class LuaTableField;
    public:
        luatable() = default;
        luatable(const luatable& rhl);
        luatable(lua_State *L);
        luatable(lua_State *L, const char *addr);
        virtual luatable& operator=(const luatable& rhl);

    public:
        LuaTableField operator[](int field);
        LuaTableField operator[](const char *field);

        int type(int field);
        int type(const char *field);

    public:
        void create(lua_State *L);
        void createMetatable(lua_State *L, const char *name);
        void setMetatable(luatable& metaTable);
        void traversal(M_TableTravCallBack callback);
        int  length();
        bool containsKey(const char* field);
        bool containsKey(int field);
        void setRawSetMode(bool mode);

    protected:
        bool RawSetMode = false;
    };
}
