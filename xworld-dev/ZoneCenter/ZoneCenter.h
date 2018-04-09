#pragma once

enum class XZoneStatus
{
    Initializing,
    Running,
    WaitWorldServerExit,
    WaitDBSave,
    Shutdown,
};

class XZoneCenter : public XWorldApp
{
public:
    virtual bool OnInit(int argc, char *argv[]) override;
    virtual void OnStartup() override;
    virtual void OnFinalize() override;
    virtual void OnQuitSignal() override;
    virtual void OnActivate() override;

    bool InitLuaEvn();
    void OnAllWorldServerExit();

public:
    zonecenter_config Config;
    std::string Guid;
    time_t LastSave = 0;
    XZoneStatus ZoneStatus = XZoneStatus::Initializing;
    bool GatedLaunch = false;

#ifdef _WIN32
private:
    XScriptReloader CommonScriptReloader{std::bind(&XZoneCenter::LoadLuaScript, this, std::placeholders::_1, true)};
    XScriptReloader CenterScriptReloader{std::bind(&XZoneCenter::LoadLuaScript, this, std::placeholders::_1, true)};
#endif
};

extern XZoneCenter *ZoneCenter;