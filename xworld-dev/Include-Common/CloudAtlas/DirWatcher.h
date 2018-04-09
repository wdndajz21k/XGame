#pragma once

#ifdef _MSC_VER

#include <string>
#include <functional>
#include <thread>
#include <windows.h>

class XDirWatcher
{
public:
    typedef std::function<void(const std::string&, uint32_t)> ProcessCallback;

    enum class WatcherState
    {
        Stopped,
        Running,
    };

    XDirWatcher();
    ~XDirWatcher();

    bool Init(const std::string& path, const ProcessCallback& callback);
    void Clear();

private:
    static std::string Utf16ToUtf8(const std::wstring& str);
    static std::wstring Utf8ToUtf16(const std::string& str);
    void ProcessNotification(const std::wstring& filename, uint32_t action);
    void Run();

    std::wstring Path;
    std::thread Thread;
    WatcherState State = WatcherState::Stopped;
    char Buffer[4096];
    HANDLE DirHandle = nullptr;
    XDirWatcher::ProcessCallback Callback;
};

#endif
