#include "stdafx.h"
#include "UserData.h"

#ifdef _WDEBUG
#define new DEBUG_NEW
#endif

bool XUserData::Load(const xuser_data& udata)
{
    assert(udata.int_value_size() == udata.int_key_size());
    assert(udata.str_value_size() == udata.str_key_size());

    for (int i = 0; i < udata.int_value_size(); ++i)
    {
        IntValue[udata.int_key(i)] = udata.int_value(i);
    }

    for (int i = 0; i < udata.str_value_size(); ++i)
    {
        StrValue[udata.str_key(i)] = udata.str_value(i);
    }

    return true;
}

void XUserData::Save(xuser_data& udata)
{
    for (auto& it : IntValue)
    {
        udata.add_int_key(it.first);
        udata.add_int_value(it.second);
    }
    for (auto& it : StrValue)
    {
        udata.add_str_key(it.first);
        udata.add_str_value(it.second);
    }
}

std::uint64_t XUserData::GetId(int group, int key)
{
    return ((std::uint64_t)group << 32) | key;
}

int XUserData::GetIntValue(std::uint64_t id)
{
    auto it = IntValue.find(id);
    int value = it != IntValue.end() ? it->second : 0;
    return value;
}

int XUserData::GetIntValue(int group, int key)
{
    return GetIntValue(GetId(group, key));
}

const char* XUserData::GetStrValue(std::uint64_t id)
{
    auto it = StrValue.find(id);
    const char *value = it != StrValue.end() ? it->second.c_str() : "";
    return value;
}

const char * XUserData::GetStrValue(int group, int key)
{
    return GetStrValue(GetId(group, key));
}

void XUserData::SetIntValue(std::uint64_t id, int value)
{
    if (value == 0)
        IntValue.erase(id);
    else
        IntValue[id] = value;
}

void XUserData::SetIntValue(int group, int key, int value)
{
    SetIntValue(GetId(group, key), value);
}

void XUserData::SetStrValue(std::uint64_t id, const char *value)
{
    if (!value || value[0] == '\0')
        StrValue.erase(id);
    else
        StrValue[id] = value;
}

void XUserData::SetStrValue(int group, int key, const char * value)
{
    SetStrValue(GetId(group, key), value);
}
