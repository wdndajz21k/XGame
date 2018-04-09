#pragma once
#ifdef _WIN32

#include "DirWatcher.h"
#include <mutex>
#include <deque>
#include <string>

class XScriptReloader
{
    typedef std::function<void(const char* fullPath)> ReloadFuncType;
public:
    XScriptReloader(const ReloadFuncType& func);

    bool Init(const std::string& path);
    void Activate();
    void PushScript(const std::string& fullPath, uint32_t action);

private:
    std::mutex Mutex;
    std::deque<std::string> ScriptQueue;
    XDirWatcher Watcher;
    ReloadFuncType ReloadFunc;
};

#endif
