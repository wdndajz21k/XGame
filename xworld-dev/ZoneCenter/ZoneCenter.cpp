#include "stdafx.h"
#include "ZoneCenter.h"

#ifdef _WDEBUG
#define new DEBUG_NEW
#endif

XZoneCenter *ZoneCenter = nullptr;


bool XZoneCenter::OnInit(int argc, char *argv[])
{
    Log("ZoneCenter Compile at: $ $", __DATE__, __TIME__);
    bool result = false;

    Guid = XCreateGuid();
    PerformanceStat->SetName("ZoneCenter", AppName.c_str());  

    XLOG_FAILED_JUMP(InitLuaEvn());


    result = true;

Exit0:
    return result;
}

void XZoneCenter::OnStartup()
{
    ActiveTimeoutWarning = true;
    GatedLaunch = Config.gated_launch();

#ifdef _WIN32
    OpenStdinThread();
#endif // _WIN32

}

void XZoneCenter::OnQuitSignal()
{
    if (ZoneStatus == XZoneStatus::Running || ZoneStatus == XZoneStatus::Initializing)
    {
        Log("Receive Quit Signal, exit ... ... ");
        ZoneStatus = XZoneStatus::WaitWorldServerExit;
    }
    else
    {
        printf("Exiting, please wait ... ...\n");
    }
}

void XZoneCenter::OnFinalize()
{
    CloseStatLog();
}

void XZoneCenter::OnActivate()
{
#ifdef _WIN32
    double cost = GetFrameCost();
    double percent = cost * XGAME_FPS / 1000;
    std::string s = XTool::Format("[ZoneCenter] $ms $%", (int)cost, (int)(percent * 100));

    SetConsoleTitleA(s.c_str());

    CommonScriptReloader.Activate();
    CenterScriptReloader.Activate();
#endif

    if (ZoneStatus == XZoneStatus::Running)
        InvokeLua("ServerEvent.Activate", CurrentFrame);

    XWorldApp::OnActivate();
}

bool XZoneCenter::InitLuaEvn()
{
#ifdef _WIN32
    CommonScriptReloader.Init("common_script");
    CenterScriptReloader.Init("center_script");
#endif

    bool result = false;

    XLuaGlobal::Init();
    mLua::luatable _G(LuaState, "_G");

    _G["FPS"] = (int)XGAME_FPS;
    result = LoadLuaScript("common_script/preload.lua", false);
    XLOG_FAILED_JUMP(result);

    result = LoadLuaScript("center_script/main.lua", false);
    XLOG_FAILED_JUMP(result);

    result = LoadLuaScriptDir("common_script");
    XLOG_FAILED_JUMP(result);

    result = LoadLuaScriptDir("center_script");
    XLOG_FAILED_JUMP(result);

Exit0:
    return result;
}

void XZoneCenter::OnAllWorldServerExit()
{
    Log("AllWorldServerExit, begin save DB ... ...");
    ZoneStatus = XZoneStatus::WaitDBSave;
}

bool InitConfig(int argc, char *argv[])
{
    bool result = ReadConfigFile("zonecenter.cfg", &ZoneCenter->Config);
    XLOG_FAILED_JUMP(result);

    if (XTool::FileExist("zonecenter_override.cfg"))
    {
        result = ReadConfigFile("zonecenter_override.cfg", &ZoneCenter->Config);
        XLOG_FAILED_JUMP(result);
    }

    result = XReadCmdConfig(argc, argv, &ZoneCenter->Config);
    XLOG_FAILED_JUMP(result);

Exit0:
    return result;
}

int main(int argc, char *argv[])
{
#ifdef _MSC_VER
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    std::string appKey = "ZoneCenter";
    ZoneCenter = new XZoneCenter();

    XLOG_FAILED_JUMP(InitConfig(argc, argv));

    if (!ZoneCenter->Config.app_key().empty())
        appKey = ZoneCenter->Config.app_key();

    ZoneCenter->Init(argc, argv, appKey.c_str(), XGAME_FPS, __DATE__);
    ZoneCenter->Run();
    ZoneCenter->Finalize();

Exit0:
    XDELETE(ZoneCenter);
    return 0;
}
