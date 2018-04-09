#pragma once

class XUserData
{
public:
    bool Load(const xuser_data& udata);
    void Save(xuser_data& udata);

public:
    static std::uint64_t GetId(int group, int key);

    int GetIntValue(std::uint64_t id);
    int GetIntValue(int group, int key);
    const char* GetStrValue(std::uint64_t id);
    const char* GetStrValue(int group, int key);
    void SetIntValue(std::uint64_t id, int value);
    void SetIntValue(int group, int key, int value);
    void SetStrValue(std::uint64_t id, const char *value);
    void SetStrValue(int group, int key, const char *value);

public:
    std::map<std::uint64_t, int> IntValue;
    std::map<std::uint64_t, std::string> StrValue;
};
