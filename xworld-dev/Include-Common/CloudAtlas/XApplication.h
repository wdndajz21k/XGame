#pragma once
#include "AppConfig.h"
#include "ModernLua++/mLua.h"

#include <thread>
#include <list>
#include <mutex>
#include <unordered_set>
#include <atomic>

class ISocketMgr;
struct lua_State;

enum class XQuitState
{
    Normal = 0,
    SendQuitSignal = 1,
    CatchedQuitSignal = 2,
};

class XApplication
{
public:
    XApplication();
    virtual ~XApplication() = default;

    virtual void Init(int argc, char *argv[], const char *defaultAppName, int fps, const char *compileTime = "");
    virtual bool OnInit(int argc, char *argv[]);
    virtual void OnStartup();
    virtual void Run();
    virtual void OnActivate();
    virtual void OnStdin(const char *s);
    virtual void OnQuitSignal();
    virtual void Exit();
    virtual void Finalize();
    virtual void OnFinalize();

    virtual double GetNetworkCost();
    virtual double GetActiveCost();
    virtual double GetFrameCost();
    virtual int GetFrame();
    virtual time_t GetTime();

    int64_t AddTimer(int64_t afterFrames, std::function<void()>&& callback);
    void CancelTimer(int64_t timerId);

    const char* GetAppName() { return AppName.c_str(); }
    void OpenStdinThread();

    void SetFrame(int frame);

public:
    int FPS = 0;
    int CurrentFrame = 0;
    double RecentActiveCost = 0;
    double RecentFrameCost = 0;
    double RecentNetworkCost = 0;
    int LastActiveCost = 0;
    time_t TimeNow = 0;
    bool Initializing = false;
    bool ActiveTimeoutWarning = false;
    std::string AppName;
    ISocketMgr *SocketMgr = nullptr;
    std::chrono::steady_clock::time_point RunTimePoint;
    std::unique_ptr<std::thread> StdinThread;
    std::list<std::string> StdinList;
    std::mutex StdinMutex;
    std::atomic<bool> ExitFlag;
    std::atomic<XQuitState> QuitSignal;
    int64_t TimerSequence = 0;
    std::map<std::pair<int64_t, int64_t>, std::function<void()>> Timers; // key: <frame, timerId>, value: callback
};

extern XApplication* AppHandle;
