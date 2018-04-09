#include "stdafx.h"
#include "WorldServer.h"

#ifdef _WDEBUG
#define new DEBUG_NEW
#endif

#if _WIN32
#   include <Iphlpapi.h>
#   pragma comment(lib, "iphlpapi.lib")
#else
#   include <arpa/inet.h>
#   include <ifaddrs.h>
#endif

XWorldServer *WorldServer = nullptr;

unsigned int GetDomainCreateTime(int domain)
{
    return (unsigned int)WorldServer->DomainCreateTime[domain];
}

static std::string GetPublicIp()
{
    XNetworkAdapterInfo* adapterInfo = QueryNetworkAdaptersInfo();
    std::vector<std::string> internalIp;
    std::vector<std::string> publicIp;
    std::string ip;

    for (XNetworkAdapterInfo* adapter = adapterInfo; adapter != NULL; adapter = adapter->pNext)
    {
        if (XTool::IsInternalIp(adapter->szAddr))
            internalIp.push_back(adapter->szAddr);
        else
            publicIp.push_back(adapter->szAddr);
    }

    if (!publicIp.empty())
        ip = publicIp[0];
    else if (!internalIp.empty())
        ip = internalIp[0];
    else
        ip = "127.0.0.1";

    FreeNetworkAdaptersQuery(adapterInfo);
    return ip;
}

bool XWorldServer::OnInit(int argc, char *argv[])
{
    Log("WorldServer Compile at: $ $", __DATE__, __TIME__);

    bool result = false;

    Log("PublicIP: $", PublicIP);
    XLOG_FAILED_JUMP(!PublicIP.empty());

    PerformanceStat->SetName("WorldServer", AppName.c_str());

    XLOG_FAILED_JUMP(InitLuaEvn());

    WorldServer->InvokeLua("main");
    result = true;
Exit0:
    return result;
}

void XWorldServer::OnStartup()
{
#ifdef _WIN32
    OpenStdinThread();
#endif // _WIN32
}

void XWorldServer::OnQuitSignal()
{
    if (WorldStatus == XWorldStatus::Exiting)
    {
        printf("Exiting, please wait ... ... \n");
    }
    else
    {
        Log("Receive Quit Signal, exit ... ... ");
        Exit();
    }
}

void XWorldServer::OnFinalize()
{
    CloseStatLog();

    Log("WorldServer Exit.");
}

void XWorldServer::OnActivate()
{
#ifdef _WIN32

    CommonScriptReloader.Activate();
    ServerScriptReloader.Activate();
#endif

    XWorldApp::OnActivate();
}

void XWorldServer::OnStdin(const char * s)
{
    if (strcmp(s, "exit") == 0 || strcmp(s, "exit\n") == 0)
    {
        DebugPlayerId = 0;
        Log("Exit client debug mode");
        return;
    }

    if (DebugPlayerId == 0)
    {
        XWorldApp::OnStdin(s);
    }
    else
    {

    }
}

bool XWorldServer::InitLuaEvn()
{
#ifdef _WIN32
    CommonScriptReloader.Init("common_script");
    ServerScriptReloader.Init("server_script");
#endif

    bool result = false;
   

Exit0:
    return result;
}

void XWorldServer::Shutdown()
{
    WorldStatus = XWorldStatus::Shutdown;
    Exit();
}

bool InitConfig(int argc, char *argv[])
{
    WorldServer->PublicIP = GetPublicIp();

    bool result = ReadConfigFile("worldserver.cfg", &WorldServer->Config);
    XLOG_FAILED_JUMP(result);

    if (XTool::FileExist("worldserver_override.cfg"))
    {
        result = ReadConfigFile("worldserver_override.cfg", &WorldServer->Config);
        XLOG_FAILED_JUMP(result);
    }

    result = XReadCmdConfig(argc, argv, &WorldServer->Config);
    XLOG_FAILED_JUMP(result);

    if (!WorldServer->Config.public_ip().empty())
        WorldServer->PublicIP = WorldServer->Config.public_ip();

Exit0:
    return result;
}

int main(int argc, char *argv[])
{
#ifdef _MSC_VER
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    std::string appKey = "WorldServer";
    WorldServer = new XWorldServer();

    XLOG_FAILED_JUMP(InitConfig(argc, argv));

    if (!WorldServer->Config.app_key().empty())
        appKey = WorldServer->Config.app_key();

    WorldServer->Init(argc, argv, appKey.c_str(), XGAME_FPS, __DATE__);
    WorldServer->Run();
    WorldServer->Finalize();

Exit0:
    XDELETE(WorldServer);
    return 0;
}
