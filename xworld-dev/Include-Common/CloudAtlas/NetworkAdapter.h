#pragma once

#define MAX_IP_STRING_SIZE      32
#define MAX_MAC_ADDR_SIZE       8

struct XNetworkAdapterInfo
{
    char        szAddr[MAX_IP_STRING_SIZE];
    char        szMask[MAX_IP_STRING_SIZE];
    int         nMacAddrLen;
    char        byMacAddr[MAX_MAC_ADDR_SIZE];
    XNetworkAdapterInfo* pNext;
};

XNetworkAdapterInfo* QueryNetworkAdaptersInfo();

void FreeNetworkAdaptersQuery(XNetworkAdapterInfo* pAdapter);
