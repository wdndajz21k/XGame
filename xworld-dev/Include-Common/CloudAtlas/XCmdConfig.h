#pragma once

class XCmdConfig
{
public:
    void AddParams(const std::string& name);
    void Parse(int argc, char* argv[]);
    const char *GetParam(const std::string& name);

protected:
    std::vector<std::string> Params;
    std::unordered_map<std::string, std::string> Results;
};
