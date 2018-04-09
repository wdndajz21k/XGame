#pragma once

#include <functional>
#include <string>
#include <sstream>
#include <typeinfo>
#include <random>
#include <string.h>
#include "google/protobuf/message.h"

#ifdef __linux__
#   include <cxxabi.h>
#endif

extern std::default_random_engine RandomEngine;

typedef std::function<bool(const char *fullPath, const char *name)> XTraverseDirFunction;
class XTool
{
public:
    static bool ReadFile(const std::string& file, std::string& s);
    static bool FileExist(const char *file);
    static std::string ToString(float f, const char *fmt);
    static bool RandomHit(double prob);
    static bool RandomHit(float prob);
    static bool RandomHit(int percent);
    static int Random(int max); // [1, max]
    static int RandomRange(int min, int max); // [min, max]
    static float Random(); // [0, 1)
    static float Random(float max); // [0, max)
    static double Random(double max); // [0, max)
    static float RandomRange(float min, float max); // [min, max)
    static double RandomRange(double min, double max); // [min, max)
    static void TraverseDir(const char *dir, XTraverseDirFunction func, const char *ext = nullptr);
    static std::string GbkToUtf8(const std::string& in);
    static std::string Utf8ToGbk(const std::string& in);
    static long long GetTickCount();
    static std::vector<std::string> SplitString(const std::string& s, char delim);
    static bool IsInternalIp(const std::string& ip); // Except 192.168.16/24 prefix
    static int GetSysPhysMemory();
    static int GetProcessMemoryUsage(int pid = 0);
    static float GetProcessCpuUsage(int pid = 0);
    static int GetLogicProcessorCount();
    static int GetUtf8Len(const char * s);
    static bool IsPureNumber(const char * s);

    template <typename KeyT, typename MsgT>
    static void GbkToUtf8(std::unordered_map<KeyT, MsgT>& container, const char *key)
    {
        const google::protobuf::Descriptor* msgDesc = MsgT::descriptor();
        const google::protobuf::FieldDescriptor *fieldDesc = msgDesc->FindFieldByName(key);
        for (auto& it : container)
        {
            google::protobuf::Message *msg = &it.second;
            if (fieldDesc->is_repeated())
            {
                for (int i = 0; i < msg->GetReflection()->FieldSize(*msg, fieldDesc); ++i)
                {
                    const std::string& gbkStr = msg->GetReflection()->GetRepeatedString(*msg, fieldDesc, i);
                    std::string utf8Str = GbkToUtf8(gbkStr);
                    msg->GetReflection()->SetRepeatedString(msg, fieldDesc, i, utf8Str);
                }
            }
            else
            {
                const std::string& gbkStr = msg->GetReflection()->GetString(*msg, fieldDesc);
                std::string utf8Str = GbkToUtf8(gbkStr);
                msg->GetReflection()->SetString(msg, fieldDesc, utf8Str);
            }
        }
    }

    template <typename ... ArgsType>
    static std::string Format(const char *fmt, ArgsType ... args)
    {
        std::stringstream ss;
        FormatStream(ss, fmt, args...);
        return ss.str();
    }

    static void FormatStream(std::stringstream& ss, const char *fmt)
    {
        ss << fmt;
    }

    template <typename Head, typename ... Tails>
    static void FormatStream(std::stringstream& ss, const char *fmt, Head head, Tails ... tails)
    {
        for (const char *p = fmt; *p != '\0'; ++p)
        {
            if (*p == '$')
            {
                std::string s;
                s.resize(p - fmt);
                memcpy((char*)s.c_str(), fmt, p - fmt);
                ss << s << head;
                FormatStream(ss, p + 1, tails...);
                break;
            }
        }
    }

    template <class T>
    static std::string GetTypeName()
    {
#ifdef __linux__
        std::unique_ptr<char, decltype(&std::free)> own(abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr), std::free);
        return own ? own.get() : typeid(T).name();
#else
        std::string name = typeid(T).name();
        name = name.substr(name.find(' ') + 1);
        return name;
#endif
    }
};
