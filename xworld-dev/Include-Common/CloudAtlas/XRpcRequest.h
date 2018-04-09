#pragma once
#include "Rpc.h"

template<typename T>
struct has_const_iterator
{
private:
    typedef char yes;
    typedef struct { char array[2]; } no;

    template<typename C> static yes test(typename C::const_iterator*);
    template<typename C> static no test(...);
public:
    static const bool value = sizeof(test<T>(0)) == sizeof(yes);
    typedef T type;
};

template <typename T>
struct has_begin_end
{
    template<typename C> static char(&f(typename std::enable_if<
        std::is_same<decltype(static_cast<typename C::const_iterator(C::*)() const>(&C::begin)),
        typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

    template<typename C> static char(&f(...))[2];

    template<typename C> static char(&g(typename std::enable_if<
        std::is_same<decltype(static_cast<typename C::const_iterator(C::*)() const>(&C::end)),
        typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

    template<typename C> static char(&g(...))[2];

    static bool const beg_value = sizeof(f<T>(0)) == 1;
    static bool const end_value = sizeof(g<T>(0)) == 1;
};

template<typename T>
struct is_container : std::integral_constant<bool, has_const_iterator<T>::value && has_begin_end<T>::beg_value && has_begin_end<T>::end_value>
{ };

#define DEFINE_XRPC_ADD_PARAM(_Type) int XRPCAddParam(char* buffer, size_t size, _Type v)

DEFINE_XRPC_ADD_PARAM(int);
DEFINE_XRPC_ADD_PARAM(unsigned int);
DEFINE_XRPC_ADD_PARAM(int64_t);
DEFINE_XRPC_ADD_PARAM(float);
DEFINE_XRPC_ADD_PARAM(double);
DEFINE_XRPC_ADD_PARAM(bool);

int XRPCAddParam(char* buffer, size_t size, const char *v);
int XRPCAddParam(char* buffer, size_t size, const std::string& v);
int XRPCAddParam(char* buffer, size_t size, XDataPak v);

template <typename ValueT>
inline int XRPCAddParam(char* buffer, size_t size, ValueT* msg)
{
    static_assert(std::is_base_of<google::protobuf::Message, ValueT>::value, "pointer params must base of protobuf::Message");
    return XRPCAddParam(buffer, size, msg->SerializeAsString());
}

template <typename Container, typename = std::enable_if_t<is_container<Container>::value
    && std::is_base_of<google::protobuf::Message, typename Container::value_type>::value>>
inline int XRPCAddParam(char* buffer, size_t size, const Container& msg)
{
    xrpc_bytes_array stringArray;
    for (const auto& element : msg)
        stringArray.add_values(element.SerializeAsString());
    return XRPCAddParam(buffer, size, stringArray.SerializeAsString());
}

inline int XRPCAddParam(char* buffer, size_t size, const std::vector<std::string>& msg)
{
    xrpc_str_array stringArray;
    for (const auto& element : msg)
        stringArray.add_values(element);
    return XRPCAddParam(buffer, size, stringArray.SerializeAsString());
}

inline int XRPCAddParam(char* buffer, size_t size, const std::vector<int>& msg)
{
    xrpc_int_array intArray;
    for (auto element : msg)
        intArray.add_values(element);
    return XRPCAddParam(buffer, size, intArray.SerializeAsString());
}

inline int XRPCMakeRequest_FillParams(char* buffer, size_t size)
{
    return 0;
}

template <typename Head, typename ...Tails>
inline int XRPCMakeRequest_FillParams(char* buffer, size_t size, Head head, Tails... tails)
{
    int headSize = XRPCAddParam(buffer, size, head);
    if (headSize == -1)
        return -1;

    int tailSize = XRPCMakeRequest_FillParams(buffer + headSize, size, tails...);
    if (tailSize == -1)
        return -1;

    return headSize + tailSize;
}

template <typename ... ArgsType>
inline int XRPCMakeRequest(char* buffer, size_t size, const char *serviceName, const char *methodName, ArgsType ... args)
{
    bool result = false;
    size_t usedSize = 0;
    size_t serviceSize = strlen(serviceName) + 1;
    size_t methodSize = strlen(methodName) + 1;
    size_t paramsSize = 0;
    XFAILED_JUMP(serviceSize + methodSize < size);

    memcpy(buffer + usedSize, serviceName, serviceSize);
    usedSize += serviceSize;
    size -= serviceSize;

    memcpy(buffer + usedSize, methodName, methodSize);
    usedSize += methodSize;
    size -= methodSize;

    paramsSize = XRPCMakeRequest_FillParams(buffer + usedSize, size, args...);
    XFAILED_JUMP(paramsSize != -1);
    usedSize += paramsSize;
    size -= paramsSize;

    result = true;

Exit0:
    return result ? usedSize : -1;
}

template<size_t N>
char* GetRpcBuffer()
{
    static std::shared_ptr<char> buffer(new char[N], [](char *p) { delete[] p; });
    return buffer.get();
}

template <typename ... ArgsType>
inline XDataPak XRPCMakeRequest(const char *serviceName, const char *methodName, ArgsType ... args)
{
    const size_t bufferSize = 8 * 1024 * 1024;
    char * buffer = GetRpcBuffer<bufferSize>();
    int size = 0;
    BEGIN_STAT("RPCMakeRequest");
    size = XRPCMakeRequest(buffer, bufferSize, serviceName, methodName, args...);
    END_STAT();
    assert(size != -1);
    return{ (const char*)buffer, (size_t)size };
}

template <typename ... ArgsType>
inline XDataPak XRPCMakeRequestWithHeader(const char *serviceName, const char *methodName, ArgsType ... args)
{
    const size_t bufferSize = 8 * 1024 * 1024;
    char * buffer = GetRpcBuffer<bufferSize>();
    int size = 0;
    BEGIN_STAT("RPCMakeRequest");
    size = XRPCMakeRequest(buffer + 4, bufferSize - 4, serviceName, methodName, args...);
    END_STAT();
    assert(size != -1);
    *(int32_t*)buffer = size;
    return{ (const char*)buffer, (size_t)size + 4 };
}
