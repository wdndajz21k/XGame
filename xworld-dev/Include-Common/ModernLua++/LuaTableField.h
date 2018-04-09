#pragma once
#include "LuaPush.h"

namespace mLua
{
    M_API class luatable;

    M_API class LuaTableField
    {
        enum class FieldType
        {
            Int,
            Str,
        };

    public:
        LuaTableField(luatable& ltable, int field);
        LuaTableField(luatable& ltable, const char *field);

    public:
        operator int();
        operator double();
        operator bool();
        operator const char*();
        operator luatable();
        operator luafunc();

        template <typename T>
        void operator =(T v)
        {
            lua_State *L = getLuaState();
            mLua::LuaStackHolder holder(L);
            PushTable();
            PushKey();
            M_Push(L, v);
            SetTable();
        }

    protected:
        lua_State* getLuaState();
        void PushTable();
        void PushField();
        void PushKey();
        void SetTable();

    protected:
        mLua::luatable& TheTable;
        FieldType TheFieldType;
        int IntKey;
        const char *StrKey;
    };
}
