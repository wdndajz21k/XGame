#pragma once
#include "Tool.h"

enum class XLogType
{
    Info,
    Warning,
    Error,
    Count,
};

// LogOpenӦ��ֻ�ڳ����Main���������һ��,Ҳ����˵,��Ҫ��dll֮��ĵط�ȥ����
// ע��Hook�����Լ���Ҫ�����̰߳�ȫ����,�����Hook�����������,��ҪС������
// �򵥵Ľ������: ��Ҫ��Hook����������дlog:)
// ��־�ļ�: ./logs/$(cszName)/$(time).log
bool LogOpen(const char cszName[], int nMaxLine);
void LogClose();

// Log����д���з�,�ڲ����Զ����!
// �����char*���ݣ����Ʒ���%hs
// �����wchar_t*���ݣ����Ʒ���%ls
void CLog(XLogType eLevel, const char cszFormat[], ...);

typedef bool(XLogCallback)(void* pvUsrData, XLogType eLevel, const char cszMsg[]);

void LogHook(void* pvUsrData, XLogCallback* pCallback);


// ʹ�÷��� : (fmt ����ʹ��$��Ϊռλ��)
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
