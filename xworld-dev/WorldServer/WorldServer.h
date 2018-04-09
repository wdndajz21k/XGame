#pragma once

enum class XWorldStatus
{
    Initializing,
    Running,
    Exiting,
    Shutdown,
};

class XWorldServer : public XWorldApp
{
public:
    virtual bool OnInit(int argc, char *argv[]) override;
    virtual void OnFinalize() override;
    virtual void OnStartup() override;
    virtual void OnQuitSignal() override;
    virtual void OnActivate() override;
    virtual void OnStdin(const char *s);

    bool InitLuaEvn();
    void Shutdown();

public:
    template <typename ... ArgsType>
    void InvokeLuaWithMe(XPlayer *player, const char *func, ArgsType ... args)
    {
        XMeHolder h(player);
        mLua::luafunc f(LuaState, func);
        f(args...);
    }
public:
    int WorldServerId = 0;
    int PlayerServerPort = 0;
    std::string Guid;
    std::string PublicIP;
    std::unordered_map<int, time_t> DomainCreateTime;
    XWorldStatus WorldStatus = XWorldStatus::Initializing;
    bool OpenGM = false;
    int DebugPlayerId = 0;

#ifdef _WIN32
private:
    XScriptReloader CommonScriptReloader{std::bind(&XWorldServer::LoadLuaScript, this, std::placeholders::_1, true)};
    XScriptReloader ServerScriptReloader{std::bind(&XWorldServer::LoadLuaScript, this, std::placeholders::_1, true)};
#endif
};

extern XWorldServer *WorldServer;
