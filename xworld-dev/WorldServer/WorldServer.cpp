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

bool XWorldServer::OnInit(int argc, char *argv[])
{
    Log("WorldServer Compile at: $ $", __DATE__, __TIME__);

    bool result = false;

    PerformanceStat->SetName("WorldServer", AppName.c_str());

    //XLOG_FAILED_JUMP(InitLuaEvn());

    //WorldServer->InvokeLua("main");
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
    //CloseStatLog();

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

int main(int argc, char *argv[])
{
#ifdef _MSC_VER
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    std::string appKey = "WorldServer";
    WorldServer = new XWorldServer();

    WorldServer->Init(argc, argv, appKey.c_str(), XGAME_FPS, __DATE__);
    WorldServer->Run();
    WorldServer->Finalize();

Exit0:
    XDELETE(WorldServer);
    return 0;
}
