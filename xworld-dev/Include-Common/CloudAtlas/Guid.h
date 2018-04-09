#pragma once
#include <string>

#ifdef __linux
#include <uuid/uuid.h>
typedef struct _GUID
{
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];
} GUID, IID;
#endif // __linux

void XCreateGuid(GUID* guid);
std::string XCreateGuid();

#ifdef __linux
bool operator == (const GUID& x, const GUID& y);
#endif

#define GUID_STRING_LEN 36 // GUID的字符串固定是这么长

bool XStringToGuid(const char* guidStr, GUID* guid);
std::string XGuidToString(const GUID& guid);
