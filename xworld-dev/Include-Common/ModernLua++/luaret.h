#pragma once
#include <vector>

namespace mLua
{
    class luatable;
    class luafunc;
    M_API class _luaret
    {
    public:
        _luaret() = default;
        _luaret(const _luaret& rhl) = delete;
        _luaret& operator=(const _luaret& rhl) = delete;
        ~_luaret();

    public:
        void clear();
        void putValue(lua_State *L, int n);
        operator int() const;
        operator double() const;
        operator bool() const;
        operator const char*() const;
        operator luatable() const;
        operator luafunc() const;

    protected:
        std::vector<int> Values;
        lua_State *ThisL = nullptr;
    };

    typedef _luaret& luaret;
}
