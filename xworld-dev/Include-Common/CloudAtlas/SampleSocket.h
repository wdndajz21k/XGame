#pragma once

class XSampleSocket
{
public:
    XSampleSocket(size_t uMaxRecvPackSize = USHRT_MAX);
    ~XSampleSocket();

    bool Connect(const char szIP[], int nPort, int nTimeout/*单位: ms*/);

    bool Send(void* pvData, size_t uDataLen);

    // 返回的是内部缓冲区,请勿释放或者保存,再次调用后会被复写
    bool Recv(char** ppbyPak, size_t* puPakSize);

    // 默认disable,单位: s
    void SetLinger(bool bEnable, int nSeconds);

    void Close();

    bool IsAlive();

    void SetRecvTimeout(int nTimeout/*单位: ms, -1为不限制超时时间,直到数据可读取为止(Blocking Socket)*/);

private:
    bool StreamSend(char* pbyData, size_t uDataLen);

private:
    SOCKET              m_nSocketHandle;
    char*               m_pbyRecvBuffer;
    size_t              m_uRecvBufferSize;
    char*               m_pbyDataBegin;
    char*               m_pbyDataEnd;
    int                 m_nTimeout;
    bool                m_bConnected;
};
