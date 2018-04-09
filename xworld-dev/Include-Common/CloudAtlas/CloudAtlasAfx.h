#pragma once

#pragma warning(disable:4091)
#pragma warning(disable:4267)
#pragma warning(disable:4503)
#pragma warning(disable:4800)
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma execution_character_set("utf-8")

#define __STDC_FORMAT_MACROS

#ifdef _MSC_VER
    #ifdef _DEBUG
        #define _WDEBUG
        #define _CRTDBG_MAP_ALLOC
        #include <stdlib.h>
        #include <crtdbg.h>
        #define DEBUG_NEW new(_CLIENT_BLOCK, __FILE__, __LINE__)
    #else
        #define DEBUG_NEW
    #endif
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif // !_WIN32_WINNT

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif // !ASIO_STANDALONE

#define _USE_MATH_DEFINES

// #include "asio.hpp"

#ifdef _MSC_VER
#include <WinSock2.h>
#include <windows.h>
#include <direct.h>
#include <io.h>
#endif
#ifdef __linux
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <stdint.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>

inline void Sleep(int ms)
{
    usleep(ms * 1000);
}

typedef float float32_t;
typedef double float64_t;

typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef int BOOL;
typedef unsigned long u_long;
typedef unsigned long ULONG;
typedef void* HANDLE;
typedef void* HMODULE;
typedef long HRESULT;

typedef int SOCKET;

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)

#if !defined(__cplusplus)
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#else
extern "C++"
{
    template <typename _CountofType, size_t _SizeOfArray>
    char(*__countof_helper(_CountofType(&_Array)[_SizeOfArray]))[_SizeOfArray];
#define _countof(_Array) sizeof(*__countof_helper(_Array))
}

#define stricmp strcasecmp
#endif

#endif


#ifdef _MSC_VER
#define RDTSC   __rdtsc
#endif

#ifdef __linux
inline uint64_t RDTSC()
{
    uint64_t uCPUTick = 0;
    uint32_t uCPUTickLow = 0;
    uint32_t uCPUTickHigh = 0;

    __asm__ __volatile__("rdtsc" : "=a" (uCPUTickLow), "=d" (uCPUTickHigh) : );

    uCPUTick = (((uint64_t)uCPUTickHigh) << 32) + uCPUTickLow;
    return uCPUTick;
}
#endif

#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <map>
#include <set>
#include <list>
#include <vector>
#include <mutex>
#include <tuple>
#include <thread>
#include <string>
#include <utility>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <cmath>
#include <array>
#include <deque>
#include <chrono>
#include <memory>
#include <cinttypes>


#include "ToolMacors.h"
#include "Guid.h"
#include "Log.h"
// #include "Network.h"
#include "Tool.h"
#include "ToolFunctions.h"
#include "PerformanceStat.h"
#include "json11.hpp"
