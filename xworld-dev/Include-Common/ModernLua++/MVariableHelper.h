#pragma once
#include "MGetLuaValue.h"

namespace mLua
{
    template <typename ValueT>
    struct CVarRead
    {
        static void Read(lua_State *L, intptr_t base, ptrdiff_t offset)
        {
            M_Push(L, *(ValueT*)(base + offset));
        }
    };

    template <int N>
    struct CVarRead < char[N] >
    {
        static void Read(lua_State *L, intptr_t base, ptrdiff_t offset)
        {
            M_Push(L, (const char*)(base + offset));
        }
    };

    template <typename ValueT>
    struct CVarWrite
    {
        static bool Write(lua_State *L, intptr_t base, ptrdiff_t offset, int n)
        {
            *(ValueT*)(base + offset) = M_GetLuaValue<ValueT>(L, n);
            return true;
        }
    };

    template <size_t N>
    struct CVarWrite < char[N] >
    {
        static bool Write(lua_State *L, intptr_t base, ptrdiff_t offset, int n)
        {
            const char *s = M_GetLuaValue<const char*>(L, n);
            if (N < strlen(s))
            {
                return false;
            }
            else
            {
                strcpy((char*)(base + offset), s);
                return true;
            }
        }
    };
}
