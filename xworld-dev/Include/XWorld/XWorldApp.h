#pragma once

class XWorldApp : public XApplication
{
public:
    XWorldApp();
    virtual ~XWorldApp();

    virtual void Init(int argc, char *argv[], const char *defaultAppName, int fps, const char *compileTime = "");
    virtual void OnActivate();
    virtual void OnStdin(const char *s);

    virtual bool LoadLuaScriptDir(const char *dir);
    virtual bool LoadLuaScript(const char *filePath, bool force);
    virtual bool ExecLuaScript(const char *lua, const char *chunkName = nullptr);

public:
    template <typename ... ArgsType>
    mLua::luaret InvokeLua(const char *func, ArgsType ... args)
    {
        TempFunc.setRef(LuaState, func);
        return TempFunc(args...);
    }

public:
    lua_State *LuaState = nullptr;
    time_t LastLuaGC = 0;
    mLua::luatable _G;
    mLua::luafunc TempFunc;
    std::unordered_set<std::string> LoadedScript;
};

extern XWorldApp* WorldAppHandle;
