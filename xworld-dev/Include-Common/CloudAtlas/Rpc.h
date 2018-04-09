#pragma once
#pragma warning(push, 3)
#include "xrpc.pb.h"
#pragma warning(pop)

struct XDataPak
{
    const char *data;
    size_t size;
    XDataPak()
        : data(nullptr), size(0)
    {

    }
    XDataPak(const char *d, size_t s)
        : data(d), size(s)
    {

    }

    XDataPak(const XDataPak& rhl)
        : data(rhl.data), size(rhl.size)
    {

    }
};
