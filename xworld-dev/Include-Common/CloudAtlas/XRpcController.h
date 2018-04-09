#pragma once
#include "Rpc.h"
#include "PerformanceStat.h"

#define RPC_API

#define XRPC_MAX_PARAMS_COUNT 12

#define XRPC_DECLARE_GETVALUE(_Type) \
    template<> inline _Type XRPCGetValue<_Type>(const char *data, size_t size, std::vector<XMessagePtr>& usedMsg, size_t *usedSize) \
    { \
        if (sizeof(_Type) <= size) { \
            *usedSize = sizeof(_Type); \
            return *(_Type*)data; \
        } \
        else { \
            return _Type(); \
        } \
    }

#define DECLARE_XRPC_CLASS(CLASSNAME) \
    public: \
        static bool XRPCRegister(XRPCController *rpcController); \
        static const char *XRPCGetClassName() \
        { \
            return #CLASSNAME; \
        } \
    private:

#define BEGIN_DECLARE_XRPC(CLASSNAME) \
    bool CLASSNAME::XRPCRegister(XRPCController *rpcController) \
    { \
        typedef CLASSNAME ClassT;

#define END_DECLARE_XRPC() return true; }

#define XRPC(NAME) rpcController->Register(XRPCGetClassName(), #NAME, &ClassT::NAME)

namespace xRpc
{
    typedef std::shared_ptr<google::protobuf::Message> XMessagePtr;

    template<typename FuncT, typename T, std::size_t... I>
    inline auto XRPCInvokeB(FuncT f, const T& t, std::integer_sequence<std::size_t, I...>) -> decltype(f(std::get<I>(t)...))
    {
        return f(std::get<I>(t)...);
    }

    template<typename FuncT, typename T>
    inline auto XRPCInvokeA(FuncT f, const T& t) -> decltype(XRPCInvokeB(f, t, std::make_index_sequence<std::tuple_size<T>::value>()))
    {
        return XRPCInvokeB(f, t, std::make_index_sequence<std::tuple_size<T>::value>());
    }

    template<typename ClassT, typename FuncT, typename T, std::size_t... I>
    inline auto XRPCThisInvokeB(ClassT *p, FuncT f, const T& t, std::integer_sequence<std::size_t, I...>) -> decltype((p->*f)(std::get<I>(t)...))
    {
        return (p->*f)(std::get<I>(t)...);
    }

    template<typename ClassT, typename FuncT, typename T>
    inline auto XRPCThisInvokeA(ClassT *p, FuncT f, const T& t) -> decltype(XRPCThisInvokeB(p, f, t, std::make_index_sequence<std::tuple_size<T>::value>()))
    {
        return XRPCThisInvokeB(p, f, t, std::make_index_sequence<std::tuple_size<T>::value>());
    }

    // RPC函数的参数类型，要么是XRPC_DECLARE_GETVALUE定义的，要么是某个google::protobuf::Message的指针（子类也可以），否则编译不懂过
    template <typename ValueT>
    inline ValueT XRPCGetValue(const char *data, size_t size, std::vector<XMessagePtr>& usedMsg, size_t *usedSize)
    {
        typedef typename std::remove_pointer<ValueT>::type MSG_TYPE;
        static_assert(std::is_base_of<google::protobuf::Message, MSG_TYPE>::value, "pointer params must base of protobuf::Message");
        XMessagePtr msgPtr;
        MSG_TYPE *msg = nullptr;
        bool result = false;
        XDataPak pak = XRPCGetValue<XDataPak>(data, size, usedMsg, usedSize);

        XFAILED_JUMP(pak.data);
        XFAILED_JUMP(pak.data[pak.size] == '\0');

        msg = new MSG_TYPE;
        XFAILED_JUMP(msg->ParseFromArray(pak.data, pak.size));
        usedMsg.push_back(XMessagePtr(msg));
        result = true;

    Exit0:
        if (!result)
        {
            *usedSize = 0;
            XDELETE(msg);
        }
        return msg;
    }

    XRPC_DECLARE_GETVALUE(int8_t)
    XRPC_DECLARE_GETVALUE(uint8_t)
    XRPC_DECLARE_GETVALUE(int16_t)
    XRPC_DECLARE_GETVALUE(uint16_t)
    XRPC_DECLARE_GETVALUE(int32_t)
    XRPC_DECLARE_GETVALUE(uint32_t)
    XRPC_DECLARE_GETVALUE(int64_t)
    XRPC_DECLARE_GETVALUE(uint64_t)
    XRPC_DECLARE_GETVALUE(bool)
    XRPC_DECLARE_GETVALUE(float)
    XRPC_DECLARE_GETVALUE(double)


    template<>
    inline XDataPak XRPCGetValue<XDataPak>(const char *data, size_t size, std::vector<XMessagePtr>& usedMsg, size_t *usedSize)
    {
        XDataPak dataPak(nullptr, 0);
        int32_t dataSize = 0;

        XFAILED_JUMP(size >= sizeof(int32_t));
        dataSize = *(int32_t*)data;
        size -= sizeof(int32_t);
        data += sizeof(int32_t);

        XFAILED_JUMP(size >= dataSize);
        *usedSize = sizeof(int32_t) + dataSize;
        dataPak.data = data;
        dataPak.size = (size_t)dataSize - 1;

    Exit0:
        return dataPak;
    }

    template<>
    inline std::string XRPCGetValue<std::string>(const char *data, size_t size, std::vector<XMessagePtr>& usedMsg, size_t *usedSize)
    {
        XDataPak pak = XRPCGetValue<XDataPak>(data, size, usedMsg, usedSize);
        return pak.data ? std::string(pak.data, pak.size) : std::string();
    }

    template<>
    inline const char* XRPCGetValue<const char*>(const char *data, size_t size, std::vector<XMessagePtr>& usedMsg, size_t *usedSize)
    {
        XDataPak pak = XRPCGetValue<XDataPak>(data, size, usedMsg, usedSize);
        return pak.data;
    }

    template <int N, typename T>
    inline bool XRPCFillParams(const char *data, size_t size, std::vector<XMessagePtr>& usedMsg, T& t)
    {
        return size == 0;
    }

    template <int N, typename T, typename Head, typename ... Tail>
    inline bool XRPCFillParams(const char *data, size_t size, std::vector<XMessagePtr>& usedMsg, T& t)
    {
        size_t usedSize = 0;
        std::get<N>(t) = XRPCGetValue<Head>(data, size, usedMsg, &usedSize);
        if (usedSize <= 0)
            return false;
        return XRPCFillParams<N + 1, T, Tail...>(data + usedSize, size - usedSize, usedMsg, t);
    }

    template <typename T, typename Head, typename ... Tail>
    inline bool XRPCFillParamsWithAttachId(size_t attachId, const char *data, size_t size, std::vector<XMessagePtr>& usedMsg, T& t)
    {
        std::get<0>(t) = (Head)attachId;
        return XRPCFillParams<1, T, Tail...>(data, size, usedMsg, t);
    }

    template <typename T>
    inline bool XRPCFillParamsWithAttachId(size_t attachId, const char *data, size_t size, std::vector<XMessagePtr>& usedMsg, T& t)
    {
        return size == 0;
    }
}

namespace xRpc
{
    class XIRPCMethodHolder
    {
    public:
        virtual ~XIRPCMethodHolder() = default;
        virtual bool Invoke(void *obj, const char *data, size_t size) = 0;
        virtual bool Invoke(void *obj, size_t connId, const char *data, size_t size) = 0;
    };

    template <typename FuncT>
    class XRPCMethodHolder : public XIRPCMethodHolder
    {
        XRPCMethodHolder() = delete;
        XRPCMethodHolder(FuncT) = delete;
    };

    template <typename FuncType>
    inline XIRPCMethodHolder* MakeRPCMethodHolder(FuncType f)
    {
        return new XRPCMethodHolder<FuncType>(f);
    }

    template <typename RetT, typename ... ArgsT>
    class XRPCMethodHolder<RetT(*)(ArgsT...)> : public XIRPCMethodHolder
    {
        typedef RetT(*FuncType)(ArgsT...);
        typedef std::tuple<ArgsT...> TupleType;
        FuncType TargetFunc;
    public:
        XRPCMethodHolder(FuncType f) : TargetFunc(f) { }
        virtual bool Invoke(void*, const char *data, size_t size) override
        {
            TupleType t;
            std::vector<XMessagePtr> usedMsgs;
            bool ret = false;
            BEGIN_STAT("RPCInvokeFillParams");
            ret = XRPCFillParams<0, TupleType, ArgsT...>(data, size, usedMsgs, t);
            END_STAT();
            if (ret)
                XRPCInvokeA(TargetFunc, t);
            return ret;
        }
        virtual bool Invoke(void*, size_t attachId, const char *data, size_t size) override
        {
            TupleType t;
            std::vector<XMessagePtr> usedMsgs;
            bool ret = false;
            BEGIN_STAT("RPCInvokeFillParams");
            ret = XRPCFillParamsWithAttachId<TupleType, ArgsT...>(attachId, data, size, usedMsgs, t);
            END_STAT();
            if (ret)
                XRPCInvokeA(TargetFunc, t);
            return ret;
        }
    };

    template <typename ClassT, typename RetT, typename ... ArgsT>
    class XRPCMethodHolder<RetT(ClassT::*)(ArgsT...)> : public XIRPCMethodHolder
    {
        typedef RetT(ClassT::*FuncType)(ArgsT...);
        typedef std::tuple<ArgsT...> TupleType;
        FuncType TargetFunc;
    public:
        XRPCMethodHolder(FuncType f) : TargetFunc(f) { }
        virtual bool Invoke(void *obj, const char *data, size_t size) override
        {
            TupleType t;
            std::vector<XMessagePtr> usedMsgs;
            bool ret = false;
            BEGIN_STAT("RPCInvokeFillParams");
            ret = XRPCFillParams<0, TupleType, ArgsT...>(data, size, usedMsgs, t);
            END_STAT();
            if (ret)
                XRPCThisInvokeA((ClassT*)obj, TargetFunc, t);
            return ret;
        }
        virtual bool Invoke(void *obj, size_t attachId, const char *data, size_t size) override
        {
            TupleType t;
            std::vector<XMessagePtr> usedMsgs;
            bool ret = false;
            BEGIN_STAT("RPCInvokeFillParams");
            ret = XRPCFillParamsWithAttachId<TupleType, ArgsT...>(attachId, data, size, usedMsgs, t);
            END_STAT();
            if (ret)
                XRPCThisInvokeA((ClassT*)obj, TargetFunc, t);
            return ret;
        }
    };
}

enum class XRpcInvokeResult
{
    SUCCEEDED = 0,
    SERVICE_UNAVAILABLE,
    METHOD_NOT_FOUND,
    DESERIALIZE_FAILED,
    DESERIALIZE_PARAMS_FAILED,
};

class XRPCController
{
public:
    struct XRPCMethodInfo
    {
        std::string MethodName;
        int ParamsCount;
        xRpc::XIRPCMethodHolder *MethodHolder;
    };

    struct XRPCService
    {
        void *ThisObject = NULL;
        std::string ClassName;
        std::function<void*(int64_t id)> GetThisFunc;
        std::unordered_map<std::string, XRPCMethodInfo> MethodMap;
    };

    struct XPerfInfo
    {
        uint64_t Count = 0;
        uint64_t Cost = 0;
        uint64_t PeakCost = 0;
        time_t PeakTime = 0;
    };

public:
    ~XRPCController();
    void Init(const std::string& logKey, bool statLog);
    int Invoke(const char *data, size_t size, size_t connId = 0, bool printLog = false);
    size_t GetInvokeInfo(const char *data, size_t size, const char **serviceName, const char **methodName);
    int FindMethod(const char *serviceName, const char *methodName, XRPCService **serviceInfo, XRPCMethodInfo **methodInfo);
    void OutputStatLog();

    template <typename RET, typename ... ArgsType>
    void Register(const char *serviceName, const char *methodName, RET(*func)(ArgsType...))
    {
        XRPCMethodInfo& info = RPCMapping[serviceName].MethodMap[methodName];
        info.MethodName = methodName;
        info.ParamsCount = sizeof...(ArgsType);
        info.MethodHolder = xRpc::MakeRPCMethodHolder(func);
    }

    template <typename ClassT, typename RET, typename ... ArgsType>
    void Register(const char *serviceName, const char *methodName, RET(ClassT::*func)(ArgsType...))
    {
        XRPCMethodInfo& info = RPCMapping[serviceName].MethodMap[methodName];
        info.MethodName = methodName;
        info.ParamsCount = sizeof...(ArgsType);
        info.MethodHolder = xRpc::MakeRPCMethodHolder(func);
    }

    template <typename ClassT>
    bool RegisterService(ClassT *thisObj = nullptr)
    {
        std::string name = XTool::GetTypeName<ClassT>();
        name = name.substr(name.find(' ') + 1);
        RPCMapping[name].ThisObject = thisObj;
        RPCMapping[name].ClassName = name;
        return ClassT::XRPCRegister(this);
    }

protected:
    bool StatFlag = false;
    XPerformanceStat PerfStat;
    std::unordered_map<std::string, XRPCService> RPCMapping;
    std::string LogKey;
};
