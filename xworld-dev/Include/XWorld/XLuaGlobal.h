#pragma once

class XLuaGlobal
{
    DECLARE_LUA_CLASS(XLuaGlobal);
public:
    static void Init();
    static int GetTime();
    static void Log(const char *log);
    static void LogWarning(const char *log);
    static void LogError(const char *log);
    static void Require(const char *file);
    static std::string GetAppKey();
    static std::string ReadFile(const char *file);
    static mLua::luatable EnumDirectory(const char *path);
    static mLua::luatable EnumDirectoryWithExt(const char *path, const char *ext);
    static int MathRandom(int min, int max);
    static bool RandomHit(int percent);
    static std::string GbkToUtf8(const char *gbkContent);
};
