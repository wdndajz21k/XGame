#pragma once
#include "Tool.h"

enum class XLogType
{
    Info,
    Warning,
    Error,
    Count,
};

// LogOpen应该只在程序的Main函数里面调一次,也就是说,不要在dll之类的地方去调用
// 注意Hook函数自己需要处理线程安全问题,如果在Hook函数里面加锁,则要小心死锁
// 简单的建议就是: 不要在Hook函数里面再写log:)
// 日志文件: ./logs/$(cszName)/$(time).log
bool LogOpen(const char cszName[], int nMaxLine);
void LogClose();

// Log无需写换行符,内部会自动添加!
// 若输出char*数据，控制符用%hs
// 若输出wchar_t*数据，控制符用%ls
void CLog(XLogType eLevel, const char cszFormat[], ...);

typedef bool(XLogCallback)(void* pvUsrData, XLogType eLevel, const char cszMsg[]);

void LogHook(void* pvUsrData, XLogCallback* pCallback);


// 使用范例 : (fmt 参数使用$作为占位符)
// Log("I'm $. I have $ years old", "a boy", 20);
template <typename ... ArgsType>
void Log(const char *fmt, ArgsType ... args)
{
    std::string log = XTool::Format(fmt, args...);
    CLog(XLogType::Info, "%s", log.c_str());
}

template <typename ... ArgsType>
void LogWarning(const char *fmt, ArgsType ... args)
{
    std::string log = XTool::Format(fmt, args...);
    CLog(XLogType::Warning, "%s", log.c_str());
}

template <typename ... ArgsType>
void LogError(const char *fmt, ArgsType ... args)
{
    std::string log = XTool::Format(fmt, args...);
    CLog(XLogType::Error, "%s", log.c_str());
}
