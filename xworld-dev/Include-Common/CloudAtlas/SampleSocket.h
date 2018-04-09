#pragma once

class XSampleSocket
{
public:
    XSampleSocket(size_t uMaxRecvPackSize = USHRT_MAX);
    ~XSampleSocket();

    bool Connect(const char szIP[], int nPort, int nTimeout/*��λ: ms*/);

    bool Send(void* pvData, size_t uDataLen);

    // ���ص����ڲ�������,�����ͷŻ��߱���,�ٴε��ú�ᱻ��д
    bool Recv(char** ppbyPak, size_t* puPakSize);

    // Ĭ��disable,��λ: s
    void SetLinger(bool bEnable, int nSeconds);

    void Close();

    bool IsAlive();

    void SetRecvTimeout(int nTimeout/*��λ: ms, -1Ϊ�����Ƴ�ʱʱ��,ֱ�����ݿɶ�ȡΪֹ(Blocking Socket)*/);

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
