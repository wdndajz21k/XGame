#include "stdafx.h"
#include "XLuaGlobal.h"
#include "XWorldApp.h"

#ifdef _WDEBUG
#define new DEBUG_NEW
#endif

BEGIN_DECLARE_SCRIPT(XLuaGlobal)
{
    LUA_STATIC_FUNC(GetTime);
    LUA_STATIC_FUNC(Log);
    LUA_STATIC_FUNC(LogWarning);
    LUA_STATIC_FUNC(LogError);
    LUA_STATIC_FUNC(Require);
    LUA_STATIC_FUNC(GetAppKey);
    LUA_STATIC_FUNC(ReadFile);
    LUA_STATIC_FUNC(EnumDirectory);
    LUA_STATIC_FUNC(EnumDirectoryWithExt);
    LUA_STATIC_FUNC(MathRandom);
    LUA_STATIC_FUNC(RandomHit);
    LUA_STATIC_FUNC(GbkToUtf8);
}
END_DECLARE_SCRIPT()

void XLuaGlobal::Init()
{
}

int XLuaGlobal::GetTime()
{
    return int(AppHandle->GetTime());
}

void XLuaGlobal::Log(const char *log)
{
    ::Log(log);
}

void XLuaGlobal::LogWarning(const char *log)
{
    ::LogWarning(log);
}

void XLuaGlobal::LogError(const char *log)
{
    ::LogError(log);
}

void XLuaGlobal::Require(const char *file)
{
    WorldAppHandle->LoadLuaScript(file, false);
}

std::string XLuaGlobal::GetAppKey()
{
    return WorldAppHandle->AppName;
}

std::string XLuaGlobal::ReadFile(const char *file)
{
    std::string s;
    XLOG_FAILED_JUMP(XTool::ReadFile(file, s));
Exit0:
    return s;
}

mLua::luatable XLuaGlobal::EnumDirectory(const char *path)
{
    return XLuaGlobal::EnumDirectoryWithExt(path, nullptr);
}

mLua::luatable XLuaGlobal::EnumDirectoryWithExt(const char *path, const char *ext)
{
    mLua::luatable t(WorldAppHandle->LuaState);
    int luaIndex = 1;

    std::set<std::string> fileSet;
    XTool::TraverseDir(path, [&](const char *fullPath, const char *name)
    {
        t[luaIndex] = fullPath;
        luaIndex++;
        return true;
    }, ext);

    return t;
}

int XLuaGlobal::MathRandom(int min, int max)
{
    return XTool::RandomRange(min, max);
}

bool XLuaGlobal::RandomHit(int percent)
{
    return XTool::RandomHit(percent);
}

std::string XLuaGlobal::GbkToUtf8(const char *gbkContent)
{
    return XTool::GbkToUtf8(gbkContent);
}
