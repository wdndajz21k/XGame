#pragma once

namespace mLua
{
    class luaref
    {
    public:
        luaref() = default;
        luaref(const luaref& rhl);
        luaref(lua_State *L, const char *addr);
        virtual luaref& operator=(const luaref& rhl);
        virtual ~luaref();

    public:
        virtual bool valid();
        virtual void clear();
        virtual void setRef(lua_State *L, const char *addr);
        virtual void setRef(lua_State *L, int n);
        virtual lua_State* getLuaState();

    public:
        virtual void Push();
        virtual void Pop();

    protected:
        int RefIndex = 0;
        lua_State *ThisL = NULL;
    };
}
