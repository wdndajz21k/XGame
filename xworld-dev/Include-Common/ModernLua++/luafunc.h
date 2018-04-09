#pragma once
#include "LuaPush.h"
#include "luaref.h"
#include "luaret.h"

namespace mLua
{
    class luafunc : public mLua::luaref
    {
    public:
        luafunc() = default;
        luafunc(const luafunc& rhl);
        luafunc(lua_State *L, const char *addr);
        virtual luafunc& operator=(const luafunc& rhl);

    public:
        template <typename ... ArgsType>
        luaret invoke(ArgsType ... args)
        {
            int nArgCount = sizeof...(args);
            Push();
            M_BatchPush(ThisL, args...);
            return DoCall(nArgCount);
        }

        template <typename ... ArgsType>
        luaret operator()(ArgsType ... args)
        {
            return invoke(args...);
        }

        virtual luaret DoCall(int nArgs);

        int GetLastErrCode();

    protected:
        int LastError = 0;
        _luaret LuaRet;
    };
}
