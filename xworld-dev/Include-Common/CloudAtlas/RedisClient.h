#pragma once
#include "SocketMgr.h"

typedef const std::vector<std::string>& REDIS_RESULT;
typedef std::function<void(bool, REDIS_RESULT)> REDIS_RESULT_CALLBACK;
typedef std::function<void(bool hasValue, const std::string&)> REDIS_HGET_RESULT_CALLBACK;

class XRedisClient
{
public:
    struct XRedisRequest
    {
        std::string Command;
        REDIS_RESULT_CALLBACK Callback;
        XRedisRequest(const std::string& cmd, const REDIS_RESULT_CALLBACK& callback)
            : Command(cmd), Callback(callback)
        {}
    };

public:
    ~XRedisClient();
    bool Init(ISocketMgr *socketMgr, const char *addr, int port, const char *pass);
    bool Connect();
    void OnSocketReceive(const void *data, size_t size);
    void OnSocketError();
    size_t ProcessBulk(const char *p, size_t size);
    bool ProcessBuffer();
    void MoveBufferOffset(size_t offset);
    void OnFinishRequest(bool success);
    void ReSendPackage();

public:
    void DoCmdList(const std::vector<std::string>& args, const REDIS_RESULT_CALLBACK& callback);

    template <typename ... ArgsType>
    void DoCmd(const REDIS_RESULT_CALLBACK& callback, ArgsType ... args)
    {
        std::stringstream ss;
        _MakeRedisCmd(ss, args...);

        SendPackage(ss.str(), callback);
    }

    template <typename TKey>
    void HGet(const char *name, TKey key, const REDIS_HGET_RESULT_CALLBACK& callback = nullptr)
    {
        DoCmd([callback](bool succeed, REDIS_RESULT result) {
            callback(!result[0].empty(), result.front());
        }, "HGET", name, key);
    }

    template <typename TKey>
    void HDel(const char *name, TKey key, const REDIS_RESULT_CALLBACK& callback = nullptr)
    {
        DoCmd(callback, "HDEL", name, key);
    }

    template <typename TKey, typename TValue>
    void HSet(const char *name, TKey key, TValue value, const REDIS_RESULT_CALLBACK& callback = nullptr)
    {
        DoCmd(callback, "HSET", name, key, value);
    }

    int __GetArgSize(int arg);
    int __GetArgSize(const char *arg);
    int __GetArgSize(const std::string& arg);
    void __MakeRedisCmd(std::stringstream& ss) { }
    template <typename Head, typename ... Tails>
    void __MakeRedisCmd(std::stringstream& ss, Head head, Tails ... tails)
    {
        ss << '$' << __GetArgSize(head) << "\r\n" << head << "\r\n";
        __MakeRedisCmd(ss, tails...);
    }
    template <typename ... ArgsType>
    void _MakeRedisCmd(std::stringstream& ss, ArgsType ... args)
    {
        ss << '*' << sizeof...(args) << "\r\n";
        __MakeRedisCmd(ss, args...);
    }

protected:
    void SendPackage(const std::string& cmd, const REDIS_RESULT_CALLBACK& callback);

public:
    ISocketMgr *SocketMgr = nullptr;
    XSocket *RedisSocket = nullptr;
    std::vector<std::pair<const char*, const char*>> TempResult;
    std::string ReceiveBuffer;
    size_t BufferUsed = 0;
    std::deque<XRedisRequest> RequestList;
    std::deque<XRedisRequest> OfflineCache;
    std::string RedisIp;
    std::string RedisPass;
    int RedisPort;
};
