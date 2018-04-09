#pragma once
#include "Rpc.h"

enum SocketUsrErr
{
    eSocketNormal = 0,
    eSocketDisconnect,
    eSocketTimeout,
    eSocketStreamErr
};

#define MAX_SOCK_PAK_SIZE (8 * 1024 * 1024)

typedef void XSocket;
typedef void (XSocketAcceptCallback)(XSocket* pListen, void* pvUsrData, XSocket* pNewSocket);
typedef void (XSocketDataCallback)(XSocket* pSocket, void* pvUsrData, char* pbyData, size_t uDataLen);
typedef void (XSocketErrorCallback)(XSocket* pSocket, void* pvUsrData, int nSysErr, int nUsrErr);

class ISocketMgr
{
public:
    virtual             ~ISocketMgr() {};
    // uMaxRecvPackSizeָӦ�ò�Recv���ݰ�ʱÿ����������С, uRecvBufferSizeָϵͳ��(TCP)�Ľ��ջ�������С
    virtual XSocket*     Listen(const char cszIP[], int nPort, size_t uMaxRecvPackSize, size_t uRecvBufferSize = 0, bool bUsePackage = true) = 0;
    virtual XSocket*     Connect(const char cszIP[], int nPort, int nTimeout /*��λ: ms*/ , size_t uMaxPackLen, size_t uRecvBufferSize = 0, bool bUsePackage = true) = 0;

    virtual void         SetAcceptCallback(XSocket* pSocket, XSocketAcceptCallback* pAcceptProc) = 0;
    virtual void         SetStreamDataCallback(XSocket* pSocket, XSocketDataCallback* pDataProc) = 0;
    virtual void         SetStreamErrorCallback(XSocket* pSocket, XSocketErrorCallback* pErrProc) = 0;

    virtual void         SetUsrData(XSocket* pSocket, void* pvUsrData) = 0;
    virtual void*        GetUsrData(XSocket* pSocket) = 0;

    virtual bool         SetSendBufferSize(XSocket* pSocket, size_t uBufferSize) = 0;
    // ��ĳЩƽ̨��,Recv buf��С��������ʱ����,�������Listen��Connectʱ����
    virtual bool         SetRecvBufferSize(XSocket* pSocket, size_t uBufferSize) = 0;

    virtual void         SetStreamTimeout(XSocket* pSocket, int nSeconds) = 0;
    virtual void         SetLinger(XSocket* pSocket, bool bEnable, int nSeconds) = 0; // Ĭ��disable,��λ: s

    virtual bool         Send(XSocket* pSocket, void* pvData, size_t uDataLen) = 0;
    virtual bool         Send(XSocket* pSocket, void* pvData[], size_t uDataLen[], size_t count) = 0;
	virtual bool         RawSend(XSocket* pSocket, void* pvData, size_t uDataLen) = 0;
    virtual void         CloseSocket(XSocket* pSocket) = 0;

    virtual const char*  GetStreamRemoteIP(XSocket* pSocket) = 0; // never return null
    virtual int          GetStreamRemotePort(XSocket* pSocket) = 0;

    virtual void         Query() = 0;

    template <typename ... ArgsType>
    inline void Rpc(XSocket *pSocket, const char *serviceName, const char *methodName, ArgsType ... args)
    {
        XDataPak request = XRPCMakeRequest(serviceName, methodName, args...);
        Send(pSocket, (void*)request.data, request.size);
    }
};

ISocketMgr* CreateSocketMgr(int nMaxListen, int nMaxStream);
