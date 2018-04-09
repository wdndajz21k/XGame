#include "stdafx.h"
#include "XWorldApp.h"

#ifdef _WDEBUG
#define new DEBUG_NEW
#endif

XWorldApp* WorldAppHandle = nullptr;

XWorldApp::XWorldApp()
{
    LuaState = mLua::M_New();
    mLua::M_Init(LuaState);
    mLua::M_SetOnErrorHandle([](const char *err) { LogError(err); });
    _G = mLua::luatable(LuaState, "_G");
    _G.setRawSetMode(true);
}

XWorldApp::~XWorldApp()
{
    _G.clear();
    TempFunc.clear();
    mLua::M_UnInit(LuaState);
    mLua::M_Destroy(LuaState);
    LuaState = nullptr;
}

void XWorldApp::Init(int argc, char *argv[], const char *defaultAppName, int fps, const char *compileTime)
{
    assert(!WorldAppHandle);
    WorldAppHandle = this;
    XApplication::Init(argc, argv, defaultAppName, fps, compileTime);
}

void XWorldApp::OnActivate()
{
    if (GetTime() - LastLuaGC > 10)
    {
        LastLuaGC = GetTime();
        lua_gc(LuaState, LUA_GCCOLLECT, 0);
    }

    XApplication::OnActivate();
}

void XWorldApp::OnStdin(const char *s)
{
    ExecLuaScript(s, "StdinLuaCmd");
}

bool XWorldApp::LoadLuaScript(const char *filePath, bool force)
{
    if (!force && LoadedScript.find(filePath) != LoadedScript.end())
        return true;

    LoadedScript.insert(filePath);
    std::string content;
    bool result = XTool::ReadFile(filePath, content);
    if (!result)
    {
        LogError("Load Lua File Failed: $", filePath);
        return false;
    }

    const char *err = mLua::M_LoadScript(LuaState, content.c_str(), filePath);
    if (err)
        LogError("[Lua] LoadScript Error: $, $", filePath, err);

    return !err;
}

bool XWorldApp::ExecLuaScript(const char *lua, const char *chunkName)
{
    const char *err = mLua::M_LoadScript(LuaState, lua, chunkName);
    if (err)
        LogError("[Lua] LoadScript Error: $, $", chunkName ? chunkName : "LuaCmd", err);
    return !err;
}

bool XWorldApp::LoadLuaScriptDir(const char *dir)
{
    mLua::LuaStackHolder h(LuaState);
    int scriptCount = 0;
    std::set<std::string> fileSet;
    XTool::TraverseDir(dir, [this, &scriptCount, &fileSet](const char *fullPath, const char *name)
    {
        scriptCount++;
        int len = strlen(fullPath);
        if (len > 4 && strcmp(fullPath + len - 4, ".lua") == 0)
            fileSet.insert(fullPath);
        return true;
    });

    bool result = false;
    // 利用set排序，保证加载顺序固定
    for (auto& path : fileSet)
    {
        result = LoadLuaScript(path.c_str(), false);
        XLOG_FAILED_JUMP(result);
    }

    Log("$ Lua Script Loaded in $.", scriptCount, dir);

Exit0:
    return result;
}
